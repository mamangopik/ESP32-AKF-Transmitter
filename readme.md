# SHMS Simon Batapa Tool Settings

## Deskripsi
Aplikasi ini digunakan untuk mengatur pengaturan alat transmitter pada perangkat SHMS Simon Batapa Politeknik Negeri Jakarta.

## Spesifikasi Hardware
- CPU: ESP32
- Sensor Accelerometer: RION AKF-394

## Parameter yang Diatur
Aplikasi ini memungkinkan pengaturan berikut:
- Kredensial WiFi
- MQTT Broker
- MQTT Topic
- Sinkronisasi RTC

## Protokol Komunikasi
- UART 115200bps
- Parity None
- 8 bit data
- 1 stop bit
- Protokol kustom antara node sensor dengan aplikasi
- Protokol RION 68

## Requirement
Pastikan Anda memiliki berikut ini sebelum menjalankan aplikasi:
- Python versi 3.7 atau yang lebih tinggi
- PyQt5
- pyqtgraph
- QT Designer
- pyserial

## Instalasi
1. Konversi file .ui menggunakan skrip `python3 convert.py`.
2. Untuk menjalankan program, gunakan perintah `python3 main.py`.
