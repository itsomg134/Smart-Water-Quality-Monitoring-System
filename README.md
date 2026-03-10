# Smart Water Quality Monitoring System

![Project Banner](https://via.placeholder.com/1200x300/0077be/ffffff?text=Smart+Water+Quality+Monitoring+System)

A comprehensive Arduino-based water quality monitoring system that measures and displays critical water parameters including temperature, turbidity, and tank water level. This project features real-time data visualization on an OLED display and is perfect for aquarium monitoring, water treatment plants, or smart home water management systems.

##  Table of Contents
- [Features](#features)
- [Hardware Components](#hardware-components)
- [System Architecture](#system-architecture)
- [Circuit Diagram](#circuit-diagram)
- [Installation](#installation)
- [Usage](#usage)
- [Code Structure](#code-structure)
- [Calibration](#calibration)
- [Troubleshooting](#troubleshooting)
- [Contributing](#contributing)
- [License](#license)

##  Features

- **Real-time Temperature Monitoring**: Measures water temperature using waterproof DS18B20 sensor
- **Water Turbidity Detection**: Monitors water clarity and particle concentration
- **Ultrasonic Water Level Sensing**: Non-contact water level measurement
- **OLED Display**: 0.96" screen showing all parameters simultaneously
- **Visual Indicators**: Color-coded warnings for abnormal readings
- **Low Power Consumption**: Optimized for continuous operation
- **Modular Design**: Easy to add more sensors or features

##  Hardware Components

| Component | Quantity | Specification | Purpose |
|-----------|----------|---------------|---------|
| Arduino Uno | 1 | Rev3 | Main microcontroller |
| Ultrasonic Sensor | 1 | HC-SR04 | Water level measurement |
| Temperature Sensor | 1 | DS18B20 (waterproof) | Water temperature monitoring |
| Turbidity Sensor | 1 | SEN0189 | Water clarity measurement |
| OLED Display | 1 | 0.96" I2C (128x64) | Data visualization |
| Breadboard | 1 | 400/800 points | Prototyping |
| Jumper Wires | 20+ | M/M, M/F | Connections |
| Resistors | 2 | 4.7kΩ (pull-up for DS18B20) | Sensor interfacing |

##  System Architecture

```
                    ┌─────────────────┐
                    │   Arduino Uno   │
                    │                 │
                    │  Digital Pins:  │
                    │   D2  - Temp    │
                    │   D3  - Trig    │
                    │   D4  - Echo    │
                    │   A0  - Turbid  │
                    │   A4/A5 - OLED  │
                    └────────┬────────┘
                             │
        ┌────────────────────┼────────────────────┐
        │                    │                    │
        ▼                    ▼                    ▼
┌───────────────┐   ┌───────────────┐   ┌───────────────┐
│   DS18B20     │   │   HC-SR04     │   │   SEN0189     │
│  Temperature  │   │  Ultrasonic   │   │  Turbidity    │
│    Sensor     │   │    Sensor     │   │    Sensor     │
└───────────────┘   └───────────────┘   └───────────────┘
        │                    │                    │
        └────────────────────┼────────────────────┘
                             │
                    ┌────────▼────────┐
                    │   0.96" OLED    │
                    │    Display      │
                    └─────────────────┘
```

##  Circuit Diagram

```
Arduino Uno Connections:
────────────────────────

DS18B20 Temperature Sensor:
├─ VCC (Red)    → 5V
├─ GND (Black)  → GND
└─ DATA (Yellow)→ Pin D2 (with 4.7kΩ pull-up to 5V)

HC-SR04 Ultrasonic Sensor:
├─ VCC          → 5V
├─ GND          → GND
├─ TRIG         → Pin D3
└─ ECHO         → Pin D4

SEN0189 Turbidity Sensor:
├─ VCC          → 5V
├─ GND          → GND
└─ OUT          → A0

0.96" OLED Display (I2C):
├─ VCC          → 5V
├─ GND          → GND
├─ SDA          → A4
└─ SCL          → A5
```

##  Installation

### Prerequisites
- [Arduino IDE](https://www.arduino.cc/en/software) (1.8.0 or higher)
- Required libraries:
  - OneWire
  - DallasTemperature
  - Adafruit SSD1306
  - Adafruit GFX

### Library Installation

1. Open Arduino IDE
2. Go to **Sketch** → **Include Library** → **Manage Libraries**
3. Search and install the following libraries:
   - `OneWire` by Paul Stoffregen
   - `DallasTemperature` by Miles Burton
   - `Adafruit SSD1306` by Adafruit
   - `Adafruit GFX` by Adafruit

### Uploading the Code

1. Clone this repository:
   ```bash
   git clone https://github.com/yourusername/water-quality-monitor.git
   ```

2. Open `water_quality_monitor.ino` in Arduino IDE

3. Select your board: **Tools** → **Board** → **Arduino Uno**

4. Select the correct port: **Tools** → **Port**

5. Click **Upload** button or press **Ctrl+U**

## Usage

1. **Power Up**: Connect the Arduino to a power source (USB or 9V adapter)
2. **Initialization**: The system will initialize all sensors (takes ~2 seconds)
3. **Display**: The OLED shows four main parameters:
   - Water temperature (°C)
   - Turbidity level (NTU/%)  
   - Water level (cm)
   - System status

4. **Reading Interpretation**:
   - **Temperature**: 0-50°C (normal: 20-30°C)
   - **Turbidity**: 0-100% (clear: <20%, cloudy: >50%)
   - **Water Level**: 0-400cm (empty: <10cm, full: >350cm)

5. **Visual Alerts**:
   -  Red screen: Critical readings
   -  Yellow screen: Warning levels
   -  Green screen: Normal operation

##  Code Structure

```
water_quality_monitor/
│
├── water_quality_monitor.ino     # Main Arduino sketch
├── README.md                      # This documentation
├── LICENSE                         # MIT License
│
├── src/
│   ├── sensors/
│   │   ├── TemperatureSensor.cpp
│   │   ├── TurbiditySensor.cpp
│   │   └── UltrasonicSensor.cpp
│   │
│   ├── display/
│   │   └── OLEDDisplay.cpp
│   │
│   └── utils/
│       └── Calibration.cpp
│
└── docs/
    ├── circuit_diagram.png
    └── wiring_guide.pdf
```

##  Calibration

### Temperature Sensor
The DS18B20 comes pre-calibrated, no adjustment needed.

### Turbidity Sensor
1. Place sensor in clean water
2. Note the analog reading (should be ~750)
3. Place in dirty water
4. Note reading (should be lower)
5. Adjust code thresholds accordingly

### Ultrasonic Sensor
1. Measure actual distance to water surface
2. Compare with sensor reading
3. Adjust speed of sound constant if needed (default: 343 m/s)

##  Troubleshooting

| Problem | Possible Solution |
|---------|------------------|
| No display | Check I2C connections; Verify OLED address |
| Temperature reads -127°C | Check pull-up resistor; Verify wiring |
| Turbidity erratic | Clean sensor; Check for air bubbles |
| Ultrasonic false readings | Reduce noise; Add delay between readings |
| Arduino not powering | Check USB cable; Try different port |

### Common Issues and Fixes

1. **OLED not detected**
   ```cpp
   // Run I2C scanner to find address
   #include <Wire.h>
   void setup() {
     Wire.begin();
     Serial.begin(9600);
     for(byte address = 1; address < 127; address++ ) {
       Wire.beginTransmission(address);
       if (Wire.endTransmission() == 0) {
         Serial.print("Found at: 0x");
         Serial.println(address, HEX);
       }
     }
   }
   ```

2. **Incorrect temperature readings**
   - Ensure 4.7kΩ pull-up resistor is properly connected
   - Check for water damage in sensor connections

##  Contributing

Contributions are welcome! Please follow these steps:

1. Fork the repository
2. Create a feature branch (`git checkout -b feature/AmazingFeature`)
3. Commit your changes (`git commit -m 'Add some AmazingFeature'`)
4. Push to the branch (`git push origin feature/AmazingFeature`)
5. Open a Pull Request

##  Future Enhancements

- [ ] Add pH sensor for acidity monitoring
- [ ] Implement WiFi connectivity for remote monitoring
- [ ] Add data logging to SD card
- [ ] Create mobile app for real-time alerts
- [ ] Add automatic water pump control
- [ ] Implement machine learning for predictive maintenance

##  License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

##  Acknowledgments

- Arduino community for excellent libraries
- Adafruit for the OLED display library
- Dallas Semiconductor for temperature sensor protocol
- All contributors and testers

## Contact

Om Gedam

GitHub: [https://github.com/itsomg134](https://github.com/itsomg134)

Email: [omgedam123098@gmail.com](mailto:omgedam123098@gmail.com)

Twitter (X): [https://twitter.com/omgedam](https://twitter.com/omgedam)

LinkedIn: [https://linkedin.com/in/omgedam](https://linkedin.com/in/omgedam)

Portfolio: [https://ogworks.lovable.app](https://ogworks.lovable.app)
