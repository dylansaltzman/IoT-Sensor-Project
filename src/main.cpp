#include <Arduino.h> // Required for PlatformIO ESP32 builds
#include <DallasTemperature.h>
#include <OneWire.h>
#include <Wire.h>
#include <Adafruit_BMP085.h>

// --- Pin Definitions ---
const int TEMP_SENSOR_PIN = 26; // OneWire Data Line (DS18B20)
const int MQ2_ANALOG_PIN = 35; // Analog ADC Input Line (MQ2)


//Set up OneWire and DallasTemperature instances
OneWire onewire (TEMP_SENSOR_PIN);
DallasTemperature sensor (&onewire);

//Initialize the BMP180 sensor
Adafruit_BMP085 bmp;

// Global Variables to hold the latest telemetry data
int rawGasValue = 0;
float currentPressureHpa = 0.0;
float currentTemperatureC = 0.0;

TaskHandle_t fastTelemetryTaskHandle = NULL; // Task handle for the fast telemetry task
TaskHandle_t slowTelemetryTaskHandle = NULL; // Task handle for the slow telemetry task

// Function to read the MQ2 gas sensor
void readGasSensor() {
  rawGasValue = analogRead(MQ2_ANALOG_PIN);
}

// Function to read the DS18B20 temperature sensor
void readTemperatureSensor() {
  currentTemperatureC = sensor.getTempCByIndex(0);
  sensor.requestTemperatures();
}

// Function to read the BMP180 pressure sensor
void readPressureSensor() {
  currentPressureHpa = bmp.readPressure() / 100.0; // Convert pascals to hectopascals
}

// --- Task 1: Fast Sensors (Gas & Pressure) ---
void fastTelemetryTask(void *pvParameters) {
  while(1) {
    // Execute fast sensor reads
    readGasSensor();
    readPressureSensor();

    // Print values instantly to the terminal, showing which core is running it
    Serial.print("[Core ");
    Serial.print(xPortGetCoreID());
    Serial.println("] ");
    
    Serial.print("Pressure (BMP180)");
    Serial.print(currentPressureHpa);
    Serial.println("hPa");
    
    Serial.print("Gas Level (MQ2)");
    Serial.print(rawGasValue);
    Serial.println("/4095");
    
    // Yield control for 50ms so this core can breathe
    vTaskDelay(pdMS_TO_TICKS(50));
  }
}

  // --- Task 2: Slow Sensors (Temperature) ---
  void slowTelemetryTask(void *pvParameters) {
    while(1) {
      // Execute slow/asynchronous sensor reads
      readTemperatureSensor();

      // Print values instantly to the terminal, showing which core is running it
      Serial.print("[Core ");
      Serial.print(xPortGetCoreID());
      Serial.println("] ");
      
      Serial.print("Temperature (DS18B20): ");
      Serial.print(currentTemperatureC);
      Serial.println("ºC");
      
      // Yield control for 750 so this core can breathe
      vTaskDelay(pdMS_TO_TICKS(750));
    }
  }

void setup() {
  Serial.begin(115200);
  //Initialize Dallas Library for DS18B20
  sensor.begin();
  sensor.setWaitForConversion(false); // Set to non-blocking mode for asynchronous temperature reads

  //Initialize I2C for BMP180
  if(!bmp.begin()){
    Serial.println("Could not find BMP180 sensor.");
    while(1){}
  }
  Serial.println("Hardware buses initialized successfully.");

  //Prime the temperature sensor for the first read
  sensor.requestTemperatures();

  //Launch the fast telemetry task on core 1 (Higher priority = 2)
  xTaskCreatePinnedToCore(
    fastTelemetryTask,   // Task function
    "Fast Telemetry Task", // Name of the task
    4096,                // Stack size in words
    NULL,                // Task input parameter
    2,                   // Priority of the task
    &fastTelemetryTaskHandle, // Task handle
    1                    // Core where the task should run
  );

  //Launch the slow telemetry task on core 0 (Lower priority = 1)
  xTaskCreatePinnedToCore(
    slowTelemetryTask,   // Task function
    "Slow Telemetry Task", // Name of the task
    4096,                // Stack size in words
    NULL,                // Task input parameter
    1,                   // Priority of the task
    &slowTelemetryTaskHandle, // Task handle
    0                    // Core where the task should run
  );
}

void loop() {
  // The FreeRTOS real-time kernel has taken full management of the hardware clock.
}
