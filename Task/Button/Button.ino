#define BTN_RED   2   
#define BTN_GREEN 21  

TaskHandle_t TaskCore0;
TaskHandle_t TaskCore1;

void setup() {
  Serial.begin(115200);

  pinMode(BTN_RED, INPUT_PULLUP);
  pinMode(BTN_GREEN, INPUT_PULLUP);

  Serial.println("=== Uji Perbandingan Core 0 dan Core 1 ===");

  xTaskCreatePinnedToCore(
    taskButtonRed,   
    "TaskRed",       
    10000,          
    NULL,         
    1,              
    &TaskCore0,      
    0               
  );

  xTaskCreatePinnedToCore(
    taskButtonGreen,
    "TaskGreen",
    10000,
    NULL,
    1,
    &TaskCore1,
    1               
  );
}

void loop() {
}

void taskButtonRed(void *pvParameters) {
  for (;;) {
    if (digitalRead(BTN_RED) == LOW) {
      Serial.print("Tombol MERAH ditekan di CORE ");
      Serial.println(xPortGetCoreID());
      delay(300);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}

void taskButtonGreen(void *pvParameters) {
  for (;;) {
    if (digitalRead(BTN_GREEN) == LOW) {
      Serial.print("Tombol HIJAU ditekan di CORE ");
      Serial.println(xPortGetCoreID());
      delay(300);
    }
    vTaskDelay(10 / portTICK_PERIOD_MS);
  }
}
