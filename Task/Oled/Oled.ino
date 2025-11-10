#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_ADDR 0x3C
#define SDA_PIN 5
#define SCL_PIN 4

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

TaskHandle_t TaskCore0;
TaskHandle_t TaskCore1;

volatile bool giliranCore0 = true; 

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("Gagal mendeteksi OLED!");
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("Perbandingan Core 0 & Core 1");
  display.display();
  delay(1500);

  xTaskCreatePinnedToCore(taskCore0, "TaskCore0", 4096, NULL, 1, &TaskCore0, 0);
  xTaskCreatePinnedToCore(taskCore1, "TaskCore1", 4096, NULL, 1, &TaskCore1, 1);
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void taskCore0(void *pvParameters) {
  for (;;) {
    if (giliranCore0) {
      display.clearDisplay();
      display.setCursor(0, 25);
      display.setTextSize(1);
      display.println("Halo dari CORE 0");
      display.display();
      Serial.printf("CORE %d menampilkan pesan\n", xPortGetCoreID());
      giliranCore0 = false; 
    }
    vTaskDelay(pdMS_TO_TICKS(2000)); 
  }
}

void taskCore1(void *pvParameters) {
  for (;;) {
    if (!giliranCore0) {
      display.clearDisplay();
      display.setCursor(0, 25);
      display.setTextSize(1);
      display.println("Halo dari CORE 1");
      display.display();
      Serial.printf("CORE %d menampilkan pesan\n", xPortGetCoreID());
      giliranCore0 = true; 
    }
    vTaskDelay(pdMS_TO_TICKS(2000)); 
  }
}
