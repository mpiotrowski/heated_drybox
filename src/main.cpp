/*
Code for the (Un)original Prusa Drybox Heater
Written by Bram Elema, Creative Commons BY-NC-SA

REQUIRES the following Arduino libraries:
 - Adafruit_GFX Library: https://github.com/adafruit/Adafruit-GFX-Library
 - Adafruit_SSD1306 Library: https://github.com/adafruit/Adafruit_SSD1306
 - DFRobot_DHT20 Sensor Library: https://github.com/DFRobot/DFRobot_DHT20
*/

#include <Arduino.h>
#include <EEPROM.h>
#include <limits.h>

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DFRobot_DHT20.h>
#include <SPI.h>
#include <Wire.h>

#include "drybox.h"
#include "screen.h"
#include "menu.h"

#define ON_OFF_BTN 6
#define UP_BTN 7
#define DOWN_BTN 8
#define HEATER_CTRL_PIN 10

DFRobot_DHT20 dht20;

unsigned long currentTime = 0; // Current time in milliseconds

unsigned long firstOnOffBtnPress = ULONG_MAX;
bool heaterOn = false; // Indicates if the heater is currently on
bool heaterRunning = false;
DeviceState deviceState = MainScreen;
MenuOption *menu = &mainScreenMenu;

ButtonPress onOffButtonPress;
ButtonPress upButtonPress;
ButtonPress downButtonPress;

float TargetTemp = 45;
unsigned short TargetHumidity = 30;
float TemperatureCalibration = 0.0;              // Calibration offset for temperature
float HumidityCalibration = 0.0;                 // Calibration offset for humidity
float Temperature = 255.0;                       // Default value for temperature, will be updated by the sensor
float Humidity = 99.0;                           // Default value for humidity, will be updated by the sensor
TemperatureUnit Unit = TemperatureUnit::Celsius; // Default temperature unit

void toggleHeater()
{
  short heaterState = LOW;
  if (heaterOn)
  {
    if (Humidity + HumidityCalibration > TargetHumidity)
    {
      if (Temperature + TemperatureCalibration < TargetTemp - 1.5)
      {
        Serial.println(F("Humidity is too high or temperature is too low"));
        // If the temperature is below the target temperature, turn on the heater
        heaterState = HIGH;
        heaterRunning = true;
      }
      else if (Temperature + TemperatureCalibration > TargetTemp + 1.5)
      {
        Serial.println(F("Temperature is too high, turning off heater"));
        // If the temperature is above the target temperature, turn off the heater
        heaterState = LOW;
        heaterRunning = false;
      }
      else
      {
        Serial.println(F("Heater is running, but conditions are not met to turn it on or off"));
      }
    }
    else
    {
      heaterState = LOW;     // Heater is off
      heaterRunning = false; // Heater is off, not running
    }
  }
  else
  {
    heaterState = LOW;     // Heater is off
    heaterRunning = false; // Heater is off, not running
  }

  digitalWrite(HEATER_CTRL_PIN, heaterState);
}

void sensorUpdate(unsigned long currentTime)
{
  static unsigned long lastSensorUpdate = 0;

  if (currentTime - lastSensorUpdate < 5000)
    return; // Update sensor data every 5 seconds

  Temperature = dht20.getTemperature(); // Get temperature in degrees Celcius
  Humidity = dht20.getHumidity() * 100; // Get relative humidity

  toggleHeater();

  lastSensorUpdate = currentTime;
}

void setup()
{
  Serial.begin(115200);

  // Initialize buttons and heater
  pinMode(ON_OFF_BTN, INPUT_PULLUP);
  pinMode(UP_BTN, INPUT_PULLUP);
  pinMode(DOWN_BTN, INPUT_PULLUP);
  pinMode(HEATER_CTRL_PIN, OUTPUT);

  // Check if sensor and display are working
  if (dht20.begin())
  {
    Serial.println("Initialize sensor failed");
    delay(1000);
  }
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println("Initialize display failed");
    delay(1000);
  }

  EEPROM.get(0, TargetTemp);             // Load target temperature from EEPROM
  EEPROM.get(4, TargetHumidity);         // Load target humidity from EEPROM
  EEPROM.get(8, TemperatureCalibration); // Load temperature calibration from EEPROM
  EEPROM.get(12, HumidityCalibration);   // Load humidity calibration from EEPROM
  EEPROM.get(16, Unit);

  // drawLogo();
  // delay(1200);
  sensorUpdate(millis());
}

void maybeUpdateEEPROM()
{
  static unsigned long lastEEPROMUpdate = 0;

  if (currentTime - lastEEPROMUpdate < 60000) // Update EEPROM every 60 seconds
    return;

  EEPROM.put(0, TargetTemp);             // Save target temperature to EEPROM
  EEPROM.put(4, TargetHumidity);         // Save target humidity to EEPROM
  EEPROM.put(8, TemperatureCalibration); // Save temperature calibration to EEPROM
  EEPROM.put(12, HumidityCalibration);   // Save humidity calibration to EEPROM
  EEPROM.put(16, Unit);                  // Save temperature display setting to EEPROM

  lastEEPROMUpdate = currentTime;
}

void buttonMenu(ButtonPress onOffPress, ButtonPress upPress, ButtonPress downPress)
{
  if (onOffPress == ButtonPress::ShortPress)
  {
    menu->onOffShortPress();
  }
  else if (onOffPress == ButtonPress::LongPress)
  {
    menu->onOffLongPress();
  }
  else if (upPress == ButtonPress::ShortPress)
  {
    menu->upPress();
  }
  else if (downPress == ButtonPress::ShortPress)
  {
    menu->downPress();
  }
}

bool b1WasPressed = false;
bool b2WasPressed = false;
bool b3WasPressed = false;

ButtonPress buttonClickHandler(int buttonState, bool &buttonPressed, unsigned long &firstPressTime, unsigned long currentTime, unsigned long delayTime = 1000)
{
  if (buttonState)
  {
    if (firstPressTime > currentTime)
    {
      firstPressTime = currentTime;
    }
    buttonPressed = true;     // Button is pressed
    return ButtonPress::None; // Still pressed, no action yet
  }
  else if (buttonPressed && delayTime == 0)
  {
    buttonPressed = false;          // Reset the button state
    return ButtonPress::ShortPress; // Short press detected
  }
  else if (buttonPressed && firstPressTime > currentTime - delayTime)
  {
    Serial.println(F("Short press detected"));
    buttonPressed = false;          // Reset the button state
    firstPressTime = ULONG_MAX;     // Reset the button press timer
    return ButtonPress::ShortPress; // Short press detected
  }
  else if (buttonPressed && firstPressTime < currentTime - delayTime)
  {
    Serial.println(F("Long press detected"));
    buttonPressed = false;         // Reset the button state
    firstPressTime = ULONG_MAX;    // Reset the button press timer
    return ButtonPress::LongPress; // Long press detected
  }
  else
  {
    buttonPressed = false;    // Reset the button state
    return ButtonPress::None; // No button press detected
  }
}

void loop()
{
  currentTime = millis();

  bool b1 = digitalRead(ON_OFF_BTN) == LOW;
  bool b2 = digitalRead(UP_BTN) == LOW;
  bool b3 = digitalRead(DOWN_BTN) == LOW;

  onOffButtonPress = buttonClickHandler(b1, b1WasPressed, firstOnOffBtnPress, currentTime);
  upButtonPress = buttonClickHandler(b2, b2WasPressed, currentTime, currentTime, 0);
  downButtonPress = buttonClickHandler(b3, b3WasPressed, currentTime, currentTime, 0);

  sensorUpdate(currentTime);
  buttonMenu(onOffButtonPress, upButtonPress, downButtonPress);

  menu->render();

  maybeUpdateEEPROM();

  delay(100); // Add a small delay to avoid overwhelming the loop
}