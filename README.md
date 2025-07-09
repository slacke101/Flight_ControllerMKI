# Flight_ControllerMKI
# Rafael Castro #

---------------------------------------------------------------------------------------------------------------------------------------------------------

````markdown
# 🛩️ STM32F405 Flight Controller

A lightweight, modular flight controller built around the **STM32F405RGT6** microcontroller. This project is designed for UAV and robotics applications requiring precise orientation, altitude, and positioning data, along with wireless telemetry support.

---------------------------------------------------------------------------------------------------------------------------------------------------------

## 🔧 Hardware Overview

**Microcontroller:**
- STM32F405RGT6 (Cortex-M4 @ 168MHz, 1MB Flash, 192KB RAM)

**Integrated Sensors & Modules:**
- **IMU**: BMI270 (6-axis inertial measurement unit over I²C)
- **Magnetometer**: QMC5883L (3-axis compass over I²C)
- **Barometer**: MPL3115A2R1 (altitude and pressure over I²C)
- **GPS Module**: ATGM332D-5N31 (UART)
- **Wireless Transceiver**: NRF24L01P-R (SPI)

---------------------------------------------------------------------------------------------------------------------------------------------------------

## 📡 Communication Interfaces

| Peripheral     | Protocol | Notes                          |
|----------------|----------|--------------------------------|
| BMI270         | I²C      | Accelerometer + Gyroscope      |
| QMC5883L       | I²C      | Magnetometer                   |
| MPL3115A2R1    | I²C      | Barometric pressure sensor     |
| ATGM332D-5N31  | UART     | GPS receiver                   |
| NRF24L01P-R    | SPI      | Wireless telemetry/RC          |

> **Note**: All sensors use I²C except GPS (UART) and NRF24L01P (SPI). Shared buses should be handled carefully.

---------------------------------------------------------------------------------------------------------------------------------------------------------

## 🧠 Features (Planned / In Progress)

- [x] Modular firmware structure (STM32CubeIDE)
- [x] I²C device integration
- [x] UART GPS support
- [ ] Sensor fusion (IMU + mag + baro)
- [ ] Telemetry communication (NRF24L01+)
- [ ] PID flight control algorithms
- [ ] SD card logging (future)
- [ ] Failsafe handling & arming logic

---------------------------------------------------------------------------------------------------------------------------------------------------------

## 🖥️ Tools Used

* STM32CubeIDE
* KiCad / EasyEDA (for schematic & PCB)
* Logic analyzer / oscilloscope (for debugging)
* VS Code (optional scripting/tooling)

---------------------------------------------------------------------------------------------------------------------------------------------------------

## 📄 License

This project is licensed under the [MIT License](LICENSE).
Feel free to use, modify, and contribute.

---------------------------------------------------------------------------------------------------------------------------------------------------------


