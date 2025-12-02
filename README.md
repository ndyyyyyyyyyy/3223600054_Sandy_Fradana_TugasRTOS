MINI-PROJECT
Production Line Counter System

Sistem penghitung barang otomatis berbasis ESP32-S3, menggunakan Interrupt, Queue, Semaphore, dan Mutex dalam lingkungan FreeRTOS. Sistem mensimulasikan proses produksi dengan pergerakan servo, pembacaan encoder sebagai penghitung item, dan tampilan OLED sebagai display utama.

Fitur Utama

Penghitungan barang otomatis menggunakan Rotary Encoder
Increment & Decrement (barang masuk / barang keluar)
Kontrol Start/Stop menggunakan tombol interrupt
Servo motor bergerak sebagai simulasi konveyor
Potensiometer untuk mengatur kecepatan servo
OLED Display menampilkan:
Status RUN / STOP
Jumlah barang (Count)
Nilai potensiometer real-time
Arsitektur FreeRTOS:
   Interrupt → Queue (Encoder)
   Interrupt → Semaphore (Button)
   Mutex OLED & Mutex Servo

Arsitektur Sistem
1. Button → Interrupt → Semaphore → Task Servo
Button Start/Stop memicu ISR
ISR memberi semaphore
Task Servo mengambil semaphore untuk mengubah state (RUN/STOP)

2. Encoder → Interrupt → Queue → Task OLED
Setiap perubahan posisi Encoder men-trigger interrupt
ISR mengirim +1 / -1 ke queue
Task OLED mengambil data queue dan update tampilan
Akses display dilindungi mutexOLED

3. Potentiometer → Task Servo

Dibaca langsung oleh Task Servo
Mengatur kecepatan putar servo
Pengaturan PWM servo dilindungi mutexServo

Hardware yang Digunakan

ESP32-S3 DevKitC-1	
Rotary Encoder	
Servo SG90	
Potentiometer	
OLED SSD1306	
Button Start/Stop	