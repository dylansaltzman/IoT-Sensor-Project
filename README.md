# IoT Sensor Project

## System Architecture & Topology
* Built a heterogeneous sensor node via Wowki which handles three separate data communication types simultaniously: Digital Bus ($I^2C$), Single Wire Bus (OneWire), and Direct Analog Signaling. 

## Hardware Interface Specifications 
| Peripheral Sensor | Sensor Pin |ESP32 GPIO Pin | Interface protocol/Notes |
| :-- | :-- | :-- | :-- |
| **BMP180** Pressure | SCL | GPIO 22 | I2C (Serial Clock - 3.3V Logic) |
| **BMP180** Pressure | SDA | GPIO 23 | I2C (Serial Data - 3.3V Logic) |
| **DS18B20** Temperature |  DQ | GPIO 26 | OneWire (Requires external 4.7kΩ pull-up to 3.3V) |
| **MQ2** Gas | AOUT | GPIO 35 | Analog (12-bit ADC Input /Max 3.3V Tolerance)
| **MQ2** Gas | VCC | 5V | 5V Power Input (required for internal heater) |
| **Other Peripherals** | VCC/GND | 3V3/GND | Main Power Distribution Rails | 

## Software Interface Specifications 
* **Libraries Used** : `<Adafruit_BMP085.h>`, `<OneWire.h>`, `<Wire.h>`, and `<DallasTemperature.h>`.
* **Data Units**: Temperature in Celsius, Pressure in Pascals, and Gas values as a raw 12-bit ADC integer from 0 to 4095.

## Bottlenecks & Metrics
* **OneWire Conversion Latency:** The DS18B20 temperature sensor is known to be slow because of its distinct internal conversion time (up to 750ms for 12-bit resolution). Utilizing synchronous `delay()` functions to wait for this telemetry may bottleneck the timing loop of the other $I^2C$ and Analog sensors which are faster.
* **ESP32 ADC Non-Linearity:** The internal Analog-to-Digital Converter (ADC) of the ESP32 is known to have non-linear characteristics near the boundary volatges (0V and 3.3V). Analog voltage attenuation trends from the MQ2 Gas sensor will require software calibration curves to maintain accuracy.
* **Bus Contention Guardrails:** If the $I^2C$ clock line experiences transient noise or hardware detachment, the firmware execution loop must include non-blocking timeout hooks to prevent total processor starvation.

## Execution & Verification Runbook
To build, compile, and run this project environment locally without cloud queue restrictions:

1. **Environment Setup:** Download and install **VS Code** along with the **PlatformIO IDE** extension.
2. **Simulation Integration:** Install the official **Wokwi** extension inside VS Code and activate user license.
3. **Repository Initialization:** Open this project root directory in VS Code. PlatformIO will automatically read the `platformio.ini` file to configure the ESP32 toolchain.
4. **Local Compilation:** Open the command palette (`Ctrl + Shift + P`), type `Wokwi: Start Simulator`, and press Enter. This compiles local source files using the machine's local processor and executes the interactive circuit canvas instantly.

## Troubleshooting Ledger

### Hardware
* The DS18B20 was not working. After examination, it requires a 4.7kΩ pull up resistor to pull the data line high and prevent floating signal data, which was missing. AFter realizing, I successfully integrated it into the canvas on Wokwi.

### Software
* Due to limitations in Wokwi, the three sensors were used instead of the BME680. In the future, the plan will be to switch to this sensor when it becomes accessible to me, either on the website or while building a prototype.