ESP32-S3 Multi-Task Peripheral Controller
Project ini mengimplementasikan kontrol berbagai peripheral menggunakan ESP32-S3 dengan sistem multi-task FreeRTOS. 
Setiap peripheral dijalankan sebagai task independen yang berjalan pada core 0 dan core 1.

✨ Fitur
Multi-task menggunakan FreeRTOS pada dual-core ESP32-S3

8 independent tasks untuk setiap peripheral

Real-time monitoring pada OLED display

Serial output untuk debugging dan monitoring

Efficient resource management dengan queue dan mutex

🔧 Komponen yang Digunakan
Komponen	Jumlah	Keterangan
ESP32-S3 DevKitC	1	Board utama
LED	3	Merah, Hijau, Orange
Push Button	2	Hijau dan Merah
Buzzer	1	Active buzzer
Potensiometer	1	Analog input
Rotary Encoder	1	KY-040
Stepper Motor	1	4-wire dengan driver
Servo Motor	1	Standard servo
OLED Display	1	SSD1306 128x64
Breadboard	1	Untuk prototyping
🔌 Pin Configuration
Peripheral	Pin ESP32-S3
LED 1 (Merah)	GPIO 19
LED 2 (Hijau)	GPIO 20
LED 3 (Orange)	GPIO 21
Button 1 (Hijau)	GPIO 37
Button 2 (Merah)	GPIO 38
Buzzer	GPIO 36
Potensiometer	GPIO 14
Encoder CLK	GPIO 41
Encoder DT	GPIO 42
Stepper A+	GPIO 15
Stepper A-	GPIO 16
Stepper B+	GPIO 7
Stepper B-	GPIO 6
Servo	GPIO 13
OLED SDA	GPIO 5
OLED SCL	GPIO 4

🏗️ Struktur Task
Core 0 Tasks:
TaskLED - Mengontrol 3 LED dengan pola binary counter

TaskBuzzer - Menghasilkan variasi frekuensi suara

TaskStepper - Menggerakkan stepper motor bolak-balik

TaskServo - Menggerakkan servo 0-180 derajat

Core 1 Tasks:
TaskButton - Membaca input dari dua push button

TaskPotentiometer - Membaca nilai analog potensiometer

TaskEncoder - Membaca rotary encoder dan menghitung pulsa

TaskOLED - Menampilkan data pada display OLED

📥 Instalasi
Prerequisites
Arduino IDE

Board ESP32-S3 support

Library yang diperlukan:

Library Required
cpp
Adafruit_GFX
Adafruit_SSD1306
ESP32Servo
AccelStepper
Steps
Clone repository ini

Buka file .ino di Arduino IDE

Install library yang diperlukan melalui Library Manager

Pilih board: ESP32S3 Dev Module

Set partition scheme: Huge APP (3MB No OTA/1MB SPIFFS)

Upload code ke ESP32-S3

🚀 Penggunaan
Setup Hardware: Sambungkan semua komponen sesuai pin configuration

Upload Code: Compile dan upload code ke ESP32-S3

Serial Monitor: Buka Serial Monitor pada 115200 baud untuk melihat output

Operasikan:

Putar potensiometer untuk melihat nilai analog

Putar encoder untuk melihat perubahan counter

Tekan tombol untuk melihat status

Amati pergerakan motor dan servo

Lihat data real-time di OLED display

📊 Output
Serial Monitor:
text
ESP32-S3 Multi-Task System Starting...
All tasks created successfully!
Button 1: PRESSED
Button 2: RELEASED
Potentiometer: 2048
Encoder Count: 5
OLED Display:
Menampilkan "Hello!" saat startup

Kemudian menampilkan:

Nilai potensiometer

Counter encoder

Waktu operasi

🔄 Skema Task
text
Core 0:
├── TaskLED (500ms)
├── TaskBuzzer (1000ms)
├── TaskStepper (Continuous)
└── TaskServo (500ms)

Core 1:
├── TaskButton (50ms)
├── TaskPotentiometer (100ms)
├── TaskEncoder (1ms)
└── TaskOLED (200ms)
👨‍💻 Author
Dibuat untuk demonstrasi multi-task programming pada ESP32-S3.
