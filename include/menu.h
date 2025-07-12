#ifndef MENU_H
#define MENU_H

#include <Arduino.h>
#include "drybox.h"

enum TemperatureUnit : char;

class MenuOption
{
public:
    virtual void enter()
    {
        Serial.println(F("Generic menu enter"));
    }

    virtual void onOffShortPress()
    {
        Serial.println(F("Generic short press action"));
    }

    virtual void onOffLongPress()
    {
        Serial.println(F("Generic long press action"));
    }

    virtual void upPress()
    {
        Serial.println(F("Generic up button action"));
    }

    virtual void downPress()
    {
        Serial.println(F("Generic down button action"));
    }

    virtual void render()
    {
        Serial.println(F("Rendering generic menu option"));
    }
};

class MainScreenMenu : public MenuOption
{
private:
    unsigned long tempSetTime = 0;

public:
    void onOffShortPress() override;
    void onOffLongPress() override;
    void upPress() override;
    void downPress() override;
    void render() override;
};

class MainSettingsMenu : public MenuOption
{
private:
    int pick = 0; // Current selection in the menu

public:
    void enter() override;
    void onOffShortPress() override;
    void onOffLongPress() override;
    void upPress() override;
    void downPress() override;
    void render() override;
};

class PickTemperatureDisplayMenu : public MenuOption
{
private:
    TemperatureUnit unit; // Default display mode
public:
    void enter() override;
    void onOffShortPress() override;
    // void onOffLongPress() override;
    void upPress() override;
    void downPress() override;
    void render() override;
};

class SetTargetTempMenu : public MenuOption
{
private:
    unsigned short targetTemp = 90; // Internal state to not affect the global TargetTemp variable directly
public:
    void enter() override;
    void onOffShortPress() override;
    // void onOffLongPress() override;
    void upPress() override;
    void downPress() override;
    // void render() override;
};

class SetTargetHumidityMenu : public MenuOption
{
private:
    unsigned short targetHumidity = 50; // Internal state to not affect the global TargetHumidity variable directly
public:
    void enter() override;
    void onOffShortPress() override;
    // void onOffLongPress() override;
    void upPress() override;
    void downPress() override;
    // void render() override;
};

class SetTemperatureCalibrationMenu : public MenuOption
{
private:
    float temperatureCalibration = 0.0; // Internal state to not affect the global TemperatureCalibration variable directly
public:
    void enter() override;
    void onOffShortPress() override;
    // void onOffLongPress() override;
    void upPress() override;
    void downPress() override;
    // void render() override;
};

class SetHumidityCalibrationMenu : public MenuOption
{
private:
    float humidityCalibration = 0.0; // Internal state to not affect the global HumidityCalibration variable directly
public:
    void enter() override;
    void onOffShortPress() override;
    // void onOffLongPress() override;
    void upPress() override;
    void downPress() override;
    // void render() override;
};

extern MainScreenMenu mainScreenMenu;
extern MainSettingsMenu mainMenu;
extern PickTemperatureDisplayMenu pickTemperatureDisplayMenu;
extern SetTargetTempMenu setTargetTempMenu;
extern SetTargetHumidityMenu setTargetHumidityMenu;
extern SetTemperatureCalibrationMenu setTemperatureCalibrationMenu;
extern SetHumidityCalibrationMenu setHumidityCalibrationMenu;

#endif // MENU_H