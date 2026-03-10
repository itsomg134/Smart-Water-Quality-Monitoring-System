#include <Arduino.h>
#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include <OneWire.h>
#include <DallasTemperature.h>

// BLE Configuration
#define SERVICE_UUID           "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID_TEMP "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID_BATTERY "beb5483e-36e1-4688-b7f5-ea07361b26a9"
#define DEVICE_NAME             "SmartWaterBottle"

// Pin Configuration
#define ONE_WIRE_BUS           4     // GPIO pin for DS18B20
#define BATTERY_PIN            34    // GPIO pin for battery voltage reading
#define LED_PIN                 2    // Built-in LED

// Temperature Sensor Setup
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

// BLE Objects
BLEServer *pServer = nullptr;
BLECharacteristic *pTempCharacteristic = nullptr;
BLECharacteristic *pBatteryCharacteristic = nullptr;
bool deviceConnected = false;

// Timing variables
unsigned long lastTempRead = 0;
unsigned long lastBatteryRead = 0;
const unsigned long tempReadInterval = 1000;  // Read temperature every 1 second
const unsigned long batteryReadInterval = 60000; // Read battery every 60 seconds

// Temperature variables
float currentTemp = 0.0f;
float minTemp = 100.0f;
float maxTemp = -100.0f;

// Battery variables
float batteryVoltage = 0.0f;
int batteryPercentage = 0;

// Server Callback Class
class MyServerCallbacks : public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
        deviceConnected = true;
        Serial.println("Device connected");
        digitalWrite(LED_PIN, HIGH);
    }

    void onDisconnect(BLEServer* pServer) {
        deviceConnected = false;
        Serial.println("Device disconnected");
        digitalWrite(LED_PIN, LOW);
        
        // Restart advertising
        pServer->getAdvertising()->start();
        Serial.println("Advertising restarted");
    }
};

// Function to read temperature
float readTemperature() {
    sensors.requestTemperatures();
    float temp = sensors.getTempCByIndex(0);
    
    // Check if reading is valid
    if (temp == DEVICE_DISCONNECTED_C) {
        Serial.println("Error: Could not read temperature data");
        return currentTemp; // Return last valid reading
    }
    
    // Update min/max temperatures
    if (temp < minTemp) minTemp = temp;
    if (temp > maxTemp) maxTemp = temp;
    
    return temp;
}

// Function to read battery voltage
float readBatteryVoltage() {
    // Read battery voltage (assuming voltage divider: 100k/100k)
    int rawValue = analogRead(BATTERY_PIN);
    float voltage = (rawValue / 4095.0) * 3.3 * 2; // Multiply by 2 due to voltage divider
    
    return voltage;
}

// Function to calculate battery percentage
int calculateBatteryPercentage(float voltage) {
    // Assuming 3.7V LiPo battery (4.2V full, 3.3V empty)
    const float fullVoltage = 4.2;
    const float emptyVoltage = 3.3;
    
    if (voltage >= fullVoltage) return 100;
    if (voltage <= emptyVoltage) return 0;
    
    return (int)((voltage - emptyVoltage) / (fullVoltage - emptyVoltage) * 100);
}

// Function to create temperature alert message
String createAlertMessage(float temp) {
    String alert = "";
    
    if (temp < 5.0) {
        alert = "TOO COLD! Water temperature is very low.";
    } else if (temp > 60.0) {
        alert = "TOO HOT! Risk of burns!";
    } else if (temp >= 35.0 && temp <= 45.0) {
        alert = "OPTIMAL: Perfect drinking temperature!";
    } else if (temp > 45.0 && temp <= 60.0) {
        alert = "WARM: Let it cool down a bit.";
    } else if (temp >= 5.0 && temp < 15.0) {
        alert = "COLD: Refreshing!";
    }
    
    return alert;
}

// Function to format temperature data for BLE transmission
String formatTemperatureData(float temp) {
    String data = "TEMP:" + String(temp, 1) + "C|";
    data += "MIN:" + String(minTemp, 1) + "C|";
    data += "MAX:" + String(maxTemp, 1) + "C|";
    data += "ALERT:" + createAlertMessage(temp);
    
    return data;
}

void setup() {
    // Initialize Serial
    Serial.begin(115200);
    Serial.println("Starting Smart Water Bottle...");
    
    // Initialize pins
    pinMode(LED_PIN, OUTPUT);
    pinMode(BATTERY_PIN, INPUT);
    
    // Initialize temperature sensor
    sensors.begin();
    
    // Initialize BLE
    BLEDevice::init(DEVICE_NAME);
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());
    
    // Create BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);
    
    // Create Temperature Characteristic
    pTempCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_TEMP,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    
    // Create Battery Characteristic
    pBatteryCharacteristic = pService->createCharacteristic(
        CHARACTERISTIC_UUID_BATTERY,
        BLECharacteristic::PROPERTY_READ |
        BLECharacteristic::PROPERTY_NOTIFY
    );
    
    // Add BLE Descriptors
    pTempCharacteristic->addDescriptor(new BLE2902());
    pBatteryCharacteristic->addDescriptor(new BLE2902());
    
    // Start the service
    pService->start();
    
    // Configure advertising
    BLEAdvertising *pAdvertising = pServer->getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(true);
    pAdvertising->setMinPreferred(0x06);  // Helps with iPhone connections
    pAdvertising->setMinPreferred(0x12);
    
    // Start advertising
    pAdvertising->start();
    
    Serial.println("BLE device is now advertising as: " + String(DEVICE_NAME));
    
    // Initial sensor readings
    currentTemp = readTemperature();
    batteryVoltage = readBatteryVoltage();
    batteryPercentage = calculateBatteryPercentage(batteryVoltage);
}

void loop() {
    unsigned long currentMillis = millis();
    
    // Read temperature at regular intervals
    if (currentMillis - lastTempRead >= tempReadInterval) {
        lastTempRead = currentMillis;
        
        // Read temperature
        float newTemp = readTemperature();
        
        // Only update if temperature changed significantly
        if (abs(newTemp - currentTemp) >= 0.1) {
            currentTemp = newTemp;
            
            // Print to Serial
            Serial.print("Temperature: ");
            Serial.print(currentTemp);
            Serial.println(" °C");
            Serial.println("Alert: " + createAlertMessage(currentTemp));
            
            // Format and send temperature data via BLE if connected
            if (deviceConnected) {
                String tempData = formatTemperatureData(currentTemp);
                pTempCharacteristic->setValue(tempData.c_str());
                pTempCharacteristic->notify();
                Serial.println("Temperature data sent via BLE");
            }
        }
    }
    
    // Read battery at regular intervals
    if (currentMillis - lastBatteryRead >= batteryReadInterval) {
        lastBatteryRead = currentMillis;
        
        // Read battery voltage
        batteryVoltage = readBatteryVoltage();
        batteryPercentage = calculateBatteryPercentage(batteryVoltage);
        
        // Print to Serial
        Serial.print("Battery: ");
        Serial.print(batteryVoltage);
        Serial.print("V (");
        Serial.print(batteryPercentage);
        Serial.println("%)");
        
        // Send battery data via BLE if connected
        if (deviceConnected) {
            String batteryData = "BATTERY:" + String(batteryVoltage, 2) + "V|";
            batteryData += "PERCENT:" + String(batteryPercentage) + "%";
            pBatteryCharacteristic->setValue(batteryData.c_str());
            pBatteryCharacteristic->notify();
        }
    }
    
    // Small delay to prevent watchdog issues
    delay(10);
}