#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <ESP32Servo.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/semphr.h"
#include "freertos/queue.h"

#define PIN_POT   14
#define PIN_SERVO 13

#define PIN_BTN_START 10
#define PIN_BTN_STOP  11

#define PIN_ENC_CLK   41
#define PIN_ENC_DT    42

// OLED
Adafruit_SSD1306 display(128, 64, &Wire);

// RTOS Objects
SemaphoreHandle_t semStart;
SemaphoreHandle_t semStop;
QueueHandle_t queueEncoder;
SemaphoreHandle_t mutexOLED;
SemaphoreHandle_t mutexServo;

// Internal State
volatile bool servoRunning = false;
volatile long encoderCount = 0;

// Servo
Servo myServo;

// Encoder state variables
volatile int lastEncState = 0;
volatile unsigned long lastEncTime = 0;
const unsigned long ENC_DEBOUNCE = 1000; // 1ms debounce

// ========================== ISR Section ==========================

void IRAM_ATTR isrStartButton() {
  xSemaphoreGiveFromISR(semStart, NULL);
}

void IRAM_ATTR isrStopButton() {
  xSemaphoreGiveFromISR(semStop, NULL);
}

void IRAM_ATTR isrEncoder() {
  if (!servoRunning) return;

  if (micros() - lastEncTime < ENC_DEBOUNCE) return;
  lastEncTime = micros();
  
  int clkState = digitalRead(PIN_ENC_CLK);
  int dtState = digitalRead(PIN_ENC_DT);
  
  if (clkState == HIGH) {
    int step = (dtState == LOW) ? 1 : -1;
    
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    xQueueSendFromISR(queueEncoder, &step, &xHigherPriorityTaskWoken);
    
    if (xHigherPriorityTaskWoken) {
      portYIELD_FROM_ISR();
    }
  }
  lastEncState = clkState;
}

// ========================== TASK: SERVO ==========================

void taskServo(void *pvParams) {
  pinMode(PIN_POT, INPUT);
  myServo.attach(PIN_SERVO);
  
  int angle = 90;
  myServo.write(angle);

  for (;;) {
    // Handle START/STOP buttons
    if (xSemaphoreTake(semStart, 0) == pdTRUE) {
      servoRunning = true;
      Serial.println("START - Servo RUNNING - Encoder ACTIVE");
    }
    if (xSemaphoreTake(semStop, 0) == pdTRUE) {
      servoRunning = false;
      Serial.println("STOP - Servo STOPPED - Encoder INACTIVE");
    }

    // Read potentiometer for speed
    int potValue = analogRead(PIN_POT);
    int speedDelay = map(potValue, 0, 4095, 20, 200);

    if (servoRunning) {
      xSemaphoreTake(mutexServo, portMAX_DELAY);
      angle = (angle + 1) % 181;
      myServo.write(angle);
      xSemaphoreGive(mutexServo);
      vTaskDelay(speedDelay / portTICK_PERIOD_MS);
    } else {
      vTaskDelay(50 / portTICK_PERIOD_MS);
    }
  }
}

// ========================== TASK: OLED ==========================

void taskOLED(void *pvParams) {
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  long lastCount = -1;
  bool lastState = false;
  int lastPotValue = -1; // Untuk melacak perubahan potentiometer

  for (;;) {
    // Process encoder queue (hanya akan ada data jika servo running)
    int step = 0;
    while (xQueueReceive(queueEncoder, &step, 0) == pdTRUE) {
      encoderCount += step;
      Serial.print("Encoder: ");
      Serial.print(step > 0 ? "KANAN" : "KIRI");
      Serial.print("(");
      Serial.print(step);
      Serial.print(") | Total: ");
      Serial.println(encoderCount);
    }

    // Baca nilai potentiometer terbaru
    int potValue = analogRead(PIN_POT);

    // Update display jika ada perubahan pada encoder, status, ATAU potentiometer
    if (encoderCount != lastCount || servoRunning != lastState || potValue != lastPotValue) {
      xSemaphoreTake(mutexOLED, portMAX_DELAY);
      
      display.clearDisplay();
      display.setCursor(0, 0);
      
      display.setTextSize(2);
      display.println("Count:");
      display.setTextSize(3);
      display.println(encoderCount);
      
      display.setTextSize(1);
      display.setCursor(0, 50);
      display.print(servoRunning ? "RUN" : "STOP");

      // Tampilkan nilai potentiometer
      display.setCursor(70, 50);
      display.print("POT:");
      display.print(potValue);
      
      display.display();
      xSemaphoreGive(mutexOLED);
      
      lastCount = encoderCount;
      lastState = servoRunning;
      lastPotValue = potValue; // Simpan nilai potentiometer terakhir
    }

    vTaskDelay(30 / portTICK_PERIOD_MS);
  }
}

// ========================== SETUP ==========================

void setup() {
  Serial.begin(115200);
  Serial.println("Starting System...");

  pinMode(PIN_BTN_START, INPUT_PULLUP);
  pinMode(PIN_BTN_STOP, INPUT_PULLUP);
  pinMode(PIN_ENC_CLK, INPUT_PULLUP);
  pinMode(PIN_ENC_DT, INPUT_PULLUP);
  
  lastEncState = digitalRead(PIN_ENC_CLK);

  attachInterrupt(digitalPinToInterrupt(PIN_BTN_START), isrStartButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_BTN_STOP), isrStopButton, FALLING);
  attachInterrupt(digitalPinToInterrupt(PIN_ENC_CLK), isrEncoder, CHANGE);

  Wire.begin(5, 4);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("OLED allocation failed!");
    while (true);
  }

  semStart = xSemaphoreCreateBinary();
  semStop = xSemaphoreCreateBinary();
  queueEncoder = xQueueCreate(50, sizeof(int));
  mutexOLED = xSemaphoreCreateMutex();
  mutexServo = xSemaphoreCreateMutex();

  xTaskCreate(taskServo, "TaskServo", 4096, NULL, 2, NULL);
  xTaskCreate(taskOLED, "TaskOLED", 4096, NULL, 1, NULL);

  Serial.println("System initialized - Potentiometer value will update in real-time on OLED");
}

void loop() {
  vTaskDelay(1000 / portTICK_PERIOD_MS);
}
