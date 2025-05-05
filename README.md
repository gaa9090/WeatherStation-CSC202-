# WeatherStation-CSC202-

Weather Station
Gwen Antonio
Matthew Bonfiglio
Jett Kopalek
Mark Luskiewicz
CSC-202: Programming Embedded Microcontrollers
Prof. Bruce Link
May 19, 2023

WeatherStation-Sim is a low-level embedded systems project that simulates a fully functional weather station using the HCS12 microcontroller. Developed using C and Assembly, this project interfaces with various components from SparkFun Electronics to display real-time weather data, demonstrating core concepts in embedded programming, sensor data handling, and microcontroller interfacing.

## Tech Stack

- **Languages**: C, Assembly
- **Microcontroller**: HCS12 (Motorola/Freescale/NXP)
- **Hardware Components**: SparkFun Electronics (temperature sensor, humidity sensor, display module, etc.)
- **Development Tools**: CodeWarrior, MiniIDE or any HCS12-compatible development environment
- **Communication Protocols**: Serial I/O, ADC (Analog to Digital Conversion)

## Installation

1. **Set up the development environment**:
   - Install CodeWarrior IDE (or any preferred IDE that supports HCS12)
   - Connect the HCS12 microcontroller to your PC using the appropriate USB-to-Serial interface

2. **Clone the repository**:
   ```bash
   git clone https://github.com/your-username/WeatherStation-Sim.git
   cd WeatherStation-Sim


## Overview
A single board integrates three core SparkFun/Dragon12 sensors to stream live climate data:

| Sensor | Metric | Port |
| ------ | ------ | ---- |
| Embedded ADC | Temperature (°C / °F) | On‑board ADC |
| Cup Anemometer | Wind speed (mph) | Port T bit 2 (edge‑interrupt) |
| Wind Vane | Wind direction (16‑point compass) | ADC A6 |

---

## User Flow
1. **Connect** the FTDI‑USB cable and open *PuTTY* (9600 8‑N‑1).
2. **Boot** the Dragon12; a welcome banner appears.
3. **Unlock:** enter keypad password `1234`.
4. **Choose** a menu option in PuTTY.
   - The choice flashes on the **7‑segment** for 1 s.
   - Then the value shows in **binary** on on‑board LEDs for 1 s.
   - Live sensor data streams to the Dragon12 **LCD**.

---

## Sensor Handling
- **Temperature** – 8‑bit ADC value → °C/°F.
- **Wind speed** – Rising‑edge timer interrupt counts rotations for 3 s; `mph = edges × 1.49`.
- **Wind direction** – ADC voltage lookup maps to one of 16 compass headings.

Timers + interrupts deliver precise 3‑second samples, while the keypad/LCD/LEDs give local feedback and PuTTY supplies a cleaner text UI than physical buttons alone.

---

## Road Map
- Add **rain gauge** for precipitation.
- Integrate **BMP180** for pressure & altitude.
- Simplify exit flow to a single **SW2** press (remove extra *Enter*).
- Polish PuTTY layout for better readability.

---


References
Argent Data Systems. (n.d.). Weather Sensor Assembly p/n 80422. Retrieved May 18, 2023, from https://cdn.sparkfun.com/assets/8/4/c/d/6/Weather_Sensor_Assembly_Updated.pdf
Freenove. (n.d.). GitHub - Freenove/Freenove_RFID_Starter_Kit_for_Raspberry_Pi: Apply to FNK0025. GitHub. https://github.com/Freenove/Freenove_RFID_Starter_Kit_for_Raspberry_Pi


MIT License © 2025


