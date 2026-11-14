#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "managers/page_manager.h"
#include "managers/sensor_manager.h"
#include "managers/wifi_config_manager.h"

// Define I2C pins for ESP32-S3 (change if your board uses different pins)
#define SDA_PIN 8
#define SCL_PIN 9

// Set the LCD address to 0x27 (common alternatives are 0x3F)
// Parameters: (I2C_address, columns, rows) -> 20 columns, 4 rows
LiquidCrystal_I2C lcd(0x27, 20, 4);

void setup()
{
  // Initialize Serial Monitor
  Serial.begin(115200);

  // Initialize custom I2C pins for ESP32-S3
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize the LCD module
  lcd.init();

  sensor_manager::initialize();
  page_manager::initialize(lcd);
}

void loop()
{
  wifi_config_manager::update();
  page_manager::update();
}