#define BUZZER_PIN 1

TaskHandle_t TaskCore0;
TaskHandle_t TaskCore1;

volatile bool giliranCore0 = true; 

void setup() {
  Serial.begin(115200);
  delay(1000);

  pinMode(BUZZER_PIN, OUTPUT);

  Serial.println("=== Perbandingan Core 0 dan Core 1 (BUZZER) ===");

  xTaskCreatePinnedToCore(
    taskCore0,
    "TaskBuzzerCore0",
    2048,
    NULL,
    1,
    &TaskCore0,
    0  
  );

  xTaskCreatePinnedToCore(
    taskCore1,
    "TaskBuzzerCore1",
    2048,
    NULL,
    1,
    &TaskCore1,
    1  
  );
}

void loop() {
  vTaskDelay(pdMS_TO_TICKS(1000));
}

void taskCore0(void *pvParameters) {
  for (;;) {
    if (giliranCore0) {
      Serial.printf("CORE %d: Buzzer bunyi lambat\n", xPortGetCoreID());
      tone(BUZZER_PIN, 1000);     
      vTaskDelay(pdMS_TO_TICKS(500));
      noTone(BUZZER_PIN);
      vTaskDelay(pdMS_TO_TICKS(500));

      giliranCore0 = false;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void taskCore1(void *pvParameters) {
  for (;;) {
    if (!giliranCore0) {
      Serial.printf("CORE %d: Buzzer bunyi cepat\n", xPortGetCoreID());
      tone(BUZZER_PIN, 2000);    
      vTaskDelay(pdMS_TO_TICKS(200));
      noTone(BUZZER_PIN);
      vTaskDelay(pdMS_TO_TICKS(200));

      giliranCore0 = true;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}
