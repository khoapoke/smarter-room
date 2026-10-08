# Environment Monitoring & Automation - ESP32 + Wokwi + PlatformIO

## 1. Hardware simulated
- ESP32 DevKitC V4
- DHT22: temperature + humidity
- Photoresistor module: dark/bright detection
- PIR: motion detection
- MQ2: gas alert via digital output
- SSD1306 OLED 128x64 I2C
- 2 relay modules: fan + light
- Buzzer: alarm

## 2. Pin map
| Function | ESP32 |
|---|---:|
| DHT22 DATA | GPIO15 |
| LDR DO | GPIO34 |
| PIR OUT | GPIO27 |
| MQ2 DO | GPIO33 |
| FAN relay IN | GPIO26 |
| LIGHT relay IN | GPIO25 |
| Buzzer + | GPIO14 |
| OLED SDA | GPIO21 |
| OLED SCL | GPIO22 |

## 3. Automation rules
1. Fan ON when temperature >= 30 C OR humidity >= 70% OR gas alert.
2. Fan OFF only after temperature < 28 C AND humidity < 65% AND no gas alert.
3. Light ON when room is dark AND motion is detected.
4. Alarm ON when gas alert OR temperature >= 35 C.

## 4. Run
### VS Code + PlatformIO
1. Install VS Code and the PlatformIO IDE extension.
2. Clone this repository and open this folder (the folder containing `platformio.ini`) in VS Code.
3. PlatformIO will download the ESP32 platform and libraries listed in `platformio.ini`.
4. Run **PlatformIO: Build**. The generated files are placed in `.pio/` and do not need to be committed.
5. For real hardware, connect an ESP32 DevKitC V4 and run **PlatformIO: Upload**, then open **PlatformIO: Monitor** at 115200 baud.

### Wokwi
1. Build the project first so `.pio/build/esp32dev/firmware.bin` and `firmware.elf` exist.
2. Open `diagram.json` in the Wokwi extension and start the simulator.
3. Click DHT22 to change temperature/humidity.
4. Click PIR -> Simulate Motion.
5. Click photoresistor to change lux.
6. Change MQ2 gas level / threshold to test the gas alarm.
7. Open Serial Monitor at 115200 baud.

The repository only needs the source code and project configuration. PlatformIO recreates `.pio/` after a fresh clone.

## 5. Important for real hardware
The MQ2 and many relay modules are commonly powered at 5V. Protect ESP32 inputs from 5V logic as appropriate (for example, with a voltage divider/level shifting) before building the physical circuit. This Wokwi version is primarily a simulation/prototype.
