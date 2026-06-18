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

void setup() {
  Serial.begin(115200);
  //Initialize Dallas Library for DS18B20
  sensor.begin();

  //Initialize I2C for BMP180
  if(!bmp.begin()){
    Serial.println("Could not find BMP180 sensor.");
    while(1){}
  }
  Serial.println("Hardware buses initialized successfully.");
}

void loop() {
  //Get OneWire Data (DS18B20)
  sensor.requestTemperatures();
  float temp = sensor.getTempCByIndex(0);
  
  //Get I2C Data (BMP180)
  float bmp180Temp = bmp.readTemperature();
  float pressurePa = bmp.readPressure();
  float pressureHpa = pressurePa/100; // Convert pascals to hectopascals

  //Get Analog Signal Data (MQ2)
  int rawGasVal = analogRead(MQ2_ANALOG_PIN);

  //Print System Telemetry to Serial Monitor
  Serial.print("\n==== SYSTEM TELEMETRY ====");
  
  Serial.print("OneWire Temperature (DS18B20): ");
  Serial.print(temp);
  Serial.println("ºC");

  Serial.print("I2C Temperature (BMP180)");
  Serial.print(bmp180Temp);
  Serial.print("ºC");

  Serial.print("I2C Pressure (BMP180)");
  Serial.print(pressureHpa);
  Serial.print("hPa");
  
  Serial.print("Analog Gas Level (MQ2)");
  Serial.print(rawGasVal);
  Serial.print("/4095");

  Serial.print("===================");
  
  // 1-second delay for OneWire data conversion
  delay(1000); 
}
