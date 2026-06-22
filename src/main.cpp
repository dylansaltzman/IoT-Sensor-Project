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

unsigned long lastTemperatureSample = 0;

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

void setup() {
  Serial.begin(115200);
  //Initialize Dallas Library for DS18B20
  sensor.setWaitForConversion(false); // Tells the library NOT to block the CPU
  
  //Initialize I2C for BMP180
  if(!bmp.begin()){
    Serial.println("Could not find BMP180 sensor.");
    while(1){}
  }
  Serial.println("Hardware buses initialized successfully.");
}

void loop() {
  // Read the MQ2 gas sensor
  readGasSensor();
  // Read the BMP180 pressure sensor
  readPressureSensor();

  if(millis() - lastTemperatureSample >= 750) { // Sample temperature every 750ms
    readTemperatureSensor();
    lastTemperatureSample = millis();

    //Print System Telemetry to Serial Monitor
  Serial.println("\n==== SYSTEM TELEMETRY ====");
  
  Serial.print("Temperature (DS18B20): ");
  Serial.print(currentTemperatureC);
  Serial.println("ºC");

  Serial.print("Pressure (BMP180)");
  Serial.print(currentPressureHpa);
  Serial.println("hPa");
  
  Serial.print("Gas Level (MQ2)");
  Serial.print(rawGasValue);
  Serial.println("/4095");

  Serial.println("===================");
  
  } 
}
