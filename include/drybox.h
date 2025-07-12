#ifndef DRYBOX_H
#define DRYBOX_H

#include <DFRobot_DHT20.h>

#include "menu.h"

enum DeviceState {
    MainScreen,
    MainMenu,
    PickTemperatureDisplay,
    SetTargetTemp,
    SetTargetHumidity,
    SetTemperatureCalibration,
    SetHumidityCalibration
};

enum TemperatureUnit : char {
    Fahrenheit = 'F',
    Celsius = 'C'
};

enum ButtonPress {
    None,
    ShortPress,
    LongPress
};

extern DFRobot_DHT20 dht20;

extern unsigned long currentTime;

// Temperature internally is always represented as Celsius, but can be displayed in Fahrenheit if defined.
extern float Temperature;
extern float Humidity;
extern float TargetTemp;
extern unsigned short TargetHumidity;
extern float TemperatureCalibration; // Calibration offset for temperature
extern float HumidityCalibration; // Calibration offset for humidity
extern TemperatureUnit Unit;

extern ButtonPress onOffButtonPress;
extern ButtonPress upButtonPress;
extern ButtonPress downButtonPress;

extern bool heaterOn;
extern bool heaterRunning;
extern DeviceState deviceState;
extern MenuOption* menu;

#endif // DRYBOX_H