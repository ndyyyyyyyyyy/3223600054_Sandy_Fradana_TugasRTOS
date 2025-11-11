#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>
#include <AccelStepper.h>

#define LED1_PIN 19
#define LED2_PIN 20
#define LED3_PIN 21
#define BTN1_PIN 37
#define BTN2_PIN 38
#define BUZZER_PIN 36
#define POT_PIN 14
#define ENCODER_CLK 41
#define ENCODER_DT 42
#define STEPPER_A_PLUS 15
#define STEPPER_A_MINUS 16
#define STEPPER_B_PLUS 7
#define STEPPER_B_MINUS 6
#define SERVO_PIN 13

Adafruit_SSD1306 display(128, 64, &Wire, -1);
Servo servo1;
AccelStepper stepper(AccelStepper::FULL4WIRE, STEPPER_A_PLUS, STEPPER_A_MINUS, STEPPER_B_PLUS, STEPPER_B_MINUS);

QueueHandle_t xQueuePot, xQueueEncoder;
SemaphoreHandle_t xMutexOLED;

void TaskLED(void *pvParameters) {
  pinMode(LED1_PIN, OUTPUT);
  pinMode(LED2_PIN, OUTPUT);
  pinMode(LED3_PIN, OUTPUT);
  int ledState = 0;
  while(1) {
    digitalWrite(LED1_PIN, ledState & 1);
    digitalWrite(LED2_PIN, ledState & 2);
    digitalWrite(LED3_PIN, ledState & 4);
    ledState = (ledState + 1) % 8;
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void TaskBuzzer(void *pvParameters) {
  pinMode(BUZZER_PIN, OUTPUT);
  int frequency = 1000;
  while(1) {
    tone(BUZZER_PIN, frequency, 100);
    frequency = (frequency > 2000) ? 1000 : frequency + 100;
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void TaskButton(void *pvParameters) {
  pinMode(BTN1_PIN, INPUT_PULLUP);
  pinMode(BTN2_PIN, INPUT_PULLUP);
  int lastBtn1 = HIGH, lastBtn2 = HIGH;
  while(1) {
    int btn1 = digitalRead(BTN1_PIN);
    int btn2 = digitalRead(BTN2_PIN);
    if (btn1 != lastBtn1) {
      Serial.printf("Button 1: %s\n", btn1 ? "RELEASED" : "PRESSED");
      lastBtn1 = btn1;
    }
    if (btn2 != lastBtn2) {
      Serial.printf("Button 2: %s\n", btn2 ? "RELEASED" : "PRESSED");
      lastBtn2 = btn2;
    }
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

void TaskPotentiometer(void *pvParameters) {
  int lastPotValue = -1;
  while(1) {
    int potValue = analogRead(POT_PIN);
    if (abs(potValue - lastPotValue) > 10) {
      xQueueSend(xQueuePot, &potValue, portMAX_DELAY);
      Serial.printf("Potentiometer: %d\n", potValue);
      lastPotValue = potValue;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void TaskEncoder(void *pvParameters) {
  pinMode(ENCODER_CLK, INPUT_PULLUP);
  pinMode(ENCODER_DT, INPUT_PULLUP);
  int lastCLK = digitalRead(ENCODER_CLK);
  int encoderCount = 0;
  int lastEncoderCount = 0;
  while(1) {
    int currentCLK = digitalRead(ENCODER_CLK);
    if (currentCLK != lastCLK) {
      if (digitalRead(ENCODER_DT) != currentCLK) {
        encoderCount++;
      } else {
        encoderCount--;
      }
      if (encoderCount != lastEncoderCount) {
        xQueueSend(xQueueEncoder, &encoderCount, portMAX_DELAY);
        Serial.printf("Encoder Count: %d\n", encoderCount);
        lastEncoderCount = encoderCount;
      }
    }
    lastCLK = currentCLK;
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

void TaskStepper(void *pvParameters) {
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(500);
  stepper.setSpeed(200);
  while(1) {
    stepper.moveTo(200);
    while(stepper.currentPosition() != 200) {
      stepper.run();
      vTaskDelay(1);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
    stepper.moveTo(0);
    while(stepper.currentPosition() != 0) {
      stepper.run();
      vTaskDelay(1);
    }
    vTaskDelay(pdMS_TO_TICKS(1000));
  }
}

void TaskServo(void *pvParameters) {
  servo1.attach(SERVO_PIN);
  int angle = 0;
  int direction = 1;
  while(1) {
    servo1.write(angle);
    angle += direction * 10;
    if (angle >= 180 || angle <= 0) {
      direction *= -1;
    }
    vTaskDelay(pdMS_TO_TICKS(500));
  }
}

void TaskOLED(void *pvParameters) {
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    while(1);
  }
  Serial.println("OLED initialized successfully");
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  int potValue = 0, encoderValue = 0;
  unsigned long lastDisplayUpdate = 0;
  if (xSemaphoreTake(xMutexOLED, portMAX_DELAY) == pdTRUE) {
    display.clearDisplay();
    display.setCursor(10, 20);
    display.println("Hello!");
    display.display();
    xSemaphoreGive(xMutexOLED);
  }
  vTaskDelay(pdMS_TO_TICKS(2000));
  while(1) {
    if (xQueueReceive(xQueuePot, &potValue, 0) == pdTRUE) {}
    if (xQueueReceive(xQueueEncoder, &encoderValue, 0) == pdTRUE) {}
    if (xSemaphoreTake(xMutexOLED, portMAX_DELAY) == pdTRUE) {
      display.clearDisplay();
      display.setTextSize(1);
      display.setCursor(0, 0);
      display.println("System Monitor");
      display.println("-------------");
      display.printf("Pot: %d\n", potValue);
      display.printf("Encoder: %d\n", encoderValue);
      display.printf("Time: %lu\n", millis()/1000);
      display.display();
      xSemaphoreGive(xMutexOLED);
    }
    vTaskDelay(pdMS_TO_TICKS(200));
  }
}

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println("ESP32-S3 Multi-Task System Starting...");
  Wire.begin(5, 4);
  xQueuePot = xQueueCreate(10, sizeof(int));
  xQueueEncoder = xQueueCreate(10, sizeof(int));
  xMutexOLED = xSemaphoreCreateMutex();
  xTaskCreatePinnedToCore(TaskLED, "TaskLED", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskBuzzer, "TaskBuzzer", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskStepper, "TaskStepper", 4096, NULL, 2, NULL, 0);
  xTaskCreatePinnedToCore(TaskServo, "TaskServo", 2048, NULL, 1, NULL, 0);
  xTaskCreatePinnedToCore(TaskButton, "TaskButton", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskPotentiometer, "TaskPotentiometer", 2048, NULL, 1, NULL, 1);
  xTaskCreatePinnedToCore(TaskEncoder, "TaskEncoder", 2048, NULL, 2, NULL, 1);
  xTaskCreatePinnedToCore(TaskOLED, "TaskOLED", 4096, NULL, 3, NULL, 1);
  Serial.println("All tasks created successfully!");
}

void loop() {
  vTaskDelete(NULL);
}
