3223600054_Sandy_Fradana_TugasRTOS

Program dan Demonstrasi RTOS pada ESP32-S3

Langkah Percobaan

Persiapan Alat dan Bahan

* Board ESP32-S3
* Sensor (contoh: Potensiometer, DHT11, atau sensor cahaya)
* Aktuator (contoh: LED, Servo, atau Buzzer)
* Breadboard dan kabel jumper
* Arduino IDE atau VSCode dengan PlatformIO

Tujuan Percobaan

* Mengoptimalkan penggunaan dua core pada ESP32-S3
* Menerapkan multitasking dengan FreeRTOS
* Membagi tugas antara dua core agar pemrosesan lebih efisien
* Mengamati bagaimana kedua core dapat menjalankan task secara paralel tanpa saling mengganggu

Konfigurasi Task FreeRTOS

* Buat dua task utama:

  1. TaskCore0 untuk membaca data sensor secara berkala
  2. TaskCore1 untuk mengontrol aktuator sesuai hasil pembacaan sensor
* Gunakan fungsi xTaskCreatePinnedToCore() untuk menempatkan task pada core yang diinginkan
* Atur prioritas task agar sistem tetap seimbang dan responsif

Pembagian Beban Antar Core

* Core 0: Bertanggung jawab pada pembacaan dan pemrosesan data sensor
* Core 1: Bertugas mengatur output serta menampilkan hasil pengolahan data

Pengujian

1. Upload program ke board ESP32-S3
2. Buka Serial Monitor untuk memantau proses eksekusi task
3. Ubah nilai input dari sensor dan amati respons aktuator secara real-time
4. Perhatikan bahwa kedua core berjalan simultan dengan pembagian beban kerja yang merata
