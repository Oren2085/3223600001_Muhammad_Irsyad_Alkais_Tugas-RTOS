# Multi-Task Paralel Semua Peripheral (ESP32-S3)
Program ini mengimplementasikan 8 task FreeRTOS pada ESP32 yang berjalan secara paralel di dua core (Core 0 & Core 1).
Masing-masing task mengontrol perangkat berbeda seperti OLED, servo, stepper, potensiometer, rotary encoder, tombol, LED, dan buzzer.

Tujuan percobaan ini adalah untuk membuktikan bahwa ESP32 dapat menjalankan beberapa proses independen secara bersamaan dengan manajemen multitasking berbasis FreeRTOS.
# Tugas per-TASK
| Task | Fungsi | Prioritas | Core | Keterangan |
| :--- | :--- | :--- | :--- | :--- |
| **OledTask** | Display Init | 1 | 0 | Menampilkan pesan "Task berjalan" saat boot, lalu task selesai. |
| **PotTask** | Analog Read | 1 | 0 | Membaca potensiometer setiap 500ms, lapor ke Serial jika berubah. |
| **ButtonTask** | Input Check | 1 | 0 | Memantau tombol (GPIO 21), lapor ke Serial saat ditekan. |
| **LedTask** | Blink 5s | 1 | 0 | LED (GPIO 2) berkedip lambat (Nyala 5s, Mati 5s). |
| **BuzzerTask** | Beep 5s | 1 | 0 | Buzzer (GPIO 1) berbunyi lambat (Nyala 5s, Mati 5s). |
| **ServoTask** | Sweep 0-180° | 2 | 1 | Menggerakkan servo bolak-balik. |
| **StepperTask** | Motor Move | 2 | 1 | Motor stepper bergerak 0-200 langkah bolak-balik. |
| **EncoderTask**| Counter/Reset| 1 | 1 | Membaca putaran encoder; tombol SW (GPIO 6) mereset hitungan. |

# Pemetaan Perangkat Keras
| Perangkat             | Pin ESP32 | Keterangan           |
| --------------------- | --------- | -------------------- |
| **OLED SDA**          | 14        | I2C data             |
| **OLED SCL**          | 13        | I2C clock            |
| **Servo Motor**       | 17        | PWM Servo            |
| **Stepper Motor IN1** | 37        | Kontrol stepper      |
| **Stepper Motor IN2** | 38        | Kontrol stepper      |
| **Stepper Motor IN3** | 39        | Kontrol stepper      |
| **Stepper Motor IN4** | 40        | Kontrol stepper      |
| **Potensiometer**     | 16        | Input analog         |
| **Rotary CLK**        | 4         | Sinyal A encoder     |
| **Rotary DT**         | 5         | Sinyal B encoder     |
| **Rotary SW**         | 6         | Tombol encoder       |
| **Push Button**       | 21        | Input tombol         |
| **LED**               | 2         | Output LED indikator |
| **Buzzer**            | 1         | Output buzzer aktif  |

# Prosedur Tes / Langkah Percobaan
## 🧪 Langkah Percobaan ESP32-S3 (Simulasi Wokwi)
| No | Langkah (Simulasi) | Hasil yang Diharapkan (di Wokwi) |
| :--- | :--- | :--- |
| 1 | **Mulai Simulasi** | Klik tombol 'Start Simulation' (segitiga hijau). |
| 2 | **Amati Inisialisasi** | Di tab **Serial Monitor**, log akan muncul, diakhiri dengan `=== FreeRTOS 8 Task Started ===`. Di **OLED Display**, pesan "Task berjalan" akan muncul sesaat. |
| 3 | **Amati LED & Buzzer** | Amati LED (GPIO 2) dan Piezo Buzzer (GPIO 1). | Keduanya akan **menyala bersamaan selama 5 detik**, lalu **mati bersamaan selama 5 detik**, berulang terus-menerus. Ini membuktikan `LedTask` dan `BuzzerTask` berjalan paralel di Core 0. |
| 4 | **Amati Aktuator** | Amati Servo Motor (GPIO 17) dan Stepper Motor (GPIO 37-40). | Servo akan bergerak bolak-balik (0°–180°). Stepper motor akan berputar maju (200 langkah) lalu mundur (ke 0) secara otomatis dan bersamaan dengan servo. Ini membuktikan `ServoTask` dan `StepperTask` berjalan di Core 1. |
| 5 | **Atur Potensiometer**| Klik Potensiometer (GPIO 16) dan geser (drag) nilainya ke atas atau ke bawah. | Serial Monitor akan mencetak `Task Potensiometer value: X%` setiap 500ms, *hanya jika* ada perubahan nilai persentase. |
| 6 | **Putar Encoder** | Klik Rotary Encoder (GPIO 4, 5) dan putar (scroll mouse atau drag melingkar). | Serial Monitor akan mencetak `Task Encoder value: N` (bertambah/berkurang) sesuai arah putaran. |
| 7 | **Tekan Tombol Encoder** | Klik dan tahan tombol tengah pada Rotary Encoder (GPIO 6). | Serial Monitor akan mencetak `Task Encoder: Direset ke 0`. |
| 8 | **Tekan Tombol** | Klik dan tahan Tombol (Pushbutton) terpisah (GPIO 21). | Serial Monitor akan mencetak `Task Button: Diklik`. |
| 9 | **Amati Keseluruhan** | Amati semua komponen secara bersamaan sambil berinteraksi dengan sensor. | Semua *task* (LED, buzzer, motor, sensor) berjalan serentak tanpa saling mengganggu, membuktikan sistem *multitasking* FreeRTOS berjalan dengan sukses. |


   # Bukti Hasil
![Uploading image.png…]()

