#include <Fonts/FreeMono9pt7b.h>

#include "drybox.h"
#include "menu.h"
#include "screen.h"

void MainScreenMenu::onOffShortPress()
{
    Serial.print(F("Toggle heater on/off setting to: "));
    heaterOn = !heaterOn;
    Serial.println(heaterOn ? F("Heater ON") : F("Heater OFF"));
}

void MainScreenMenu::onOffLongPress()
{
    deviceState = MainMenu;
    menu = &mainMenu;
    menu->enter(); // Call enter to reset the menu state
}

void MainScreenMenu::upPress()
{
    float step = 1.0; // Step size for temperature adjustment
    if(Unit == TemperatureUnit::Celsius)
    {
        step = 1.0; // Smaller step for Celsius
    }
    else
    {
        step = 5.0/9.0; // Adjust step for Fahrenheit
    }
    TargetTemp = min(TargetTemp + step, 50); // Limit target temperature to a maximum of 50
    Serial.print(F("Target temperature set to: "));
    Serial.println(TargetTemp);
}

void MainScreenMenu::downPress()
{
    float step = 1.0; // Step size for temperature adjustment
    if(Unit == TemperatureUnit::Celsius)
    {
        step = 0.5; // Smaller step for Celsius
    }
    else
    {
        step = 5.0/9.0; // Adjust step for Fahrenheit
    }
    TargetTemp = max(TargetTemp - step, 0); // Limit target temperature to a minimum of 0
}

void MainScreenMenu::render()
{
    prepareScreen();
    display.setTextSize(2);
    char tempStr[7];
    formatTemperature(Temperature, false, tempStr, sizeof(tempStr));
    char humStr[6];
    formatHumidity(Humidity, humStr, sizeof(humStr));
    display.setCursor(0, 24);
    display.print(tempStr);
    if(heaterRunning)
    {
        display.print('^');
    }
    display.setCursor(0, 48);
    display.print(humStr);
    display.display();
}

void MainSettingsMenu::enter()
{
    Serial.println(F("Entering main settings menu"));
    pick = 0; // Reset the pick to the first option
}

void MainSettingsMenu::onOffShortPress()
{
    switch (pick)
    {
    case 0: // Temperature Display
        deviceState = PickTemperatureDisplay;
        menu = &pickTemperatureDisplayMenu;
        break;
    case 1: // Set Target Temperature
        deviceState = SetTargetTemp;
        menu = &setTargetTempMenu;
        break;
    case 2: // Set Target Humidity
        deviceState = SetTargetHumidity;
        menu = &setTargetHumidityMenu;
        break;
    case 3: // Set Temperature Calibration
        deviceState = SetTemperatureCalibration;
        menu = &setTemperatureCalibrationMenu;
        break;
    case 4: // Set Humidity Calibration
        deviceState = SetHumidityCalibration;
        menu = &setHumidityCalibrationMenu;
        break;

    default:
        break;
    }
    menu->enter(); // Call enter to reset the menu state
}

void MainSettingsMenu::onOffLongPress()
{
    Serial.println(F("Exiting main menu"));
    deviceState = MainScreen;
    menu = &mainScreenMenu;
    menu->enter(); // Call enter to reset the menu state
}

void MainSettingsMenu::upPress()
{
    Serial.println(F("Up button pressed in main menu"));
    pick--;
    if (pick < 0)
        pick = 4; // Wrap around to the last option
}

void MainSettingsMenu::downPress()
{
    Serial.println(F("Down button pressed in main menu"));
    pick++;
    if (pick > 4)
        pick = 0; // Wrap around to the first option
}

void MainSettingsMenu::render()
{
    prepareScreen();
    display.setTextSize(1);
    display.setFont(&FreeMono9pt7b);
    display.setCursor(0, 32);
    display.print(F("Menu"));
    display.setFont(NULL);
    display.setCursor(0, 48);

    switch (pick)
    {
    case 0: // Temperature Display
        display.print(F("1) Temp Unit"));
        break;
    case 1: // Set Target Temperature
        display.print(F("2) Target Temp"));
        break;
    case 2: // Set Target Humidity
        display.print(F("3) Target Hum"));
        break;
    case 3: // Set Temperature Calibration
        display.print(F("4) Temp Calib"));
        break;
    case 4: // Set Humidity Calibration
        display.print(F("5) Hum Calib"));
        break;
    default:
        display.print(F("Invalid Option"));
        break;
    }
    display.display();
}

void PickTemperatureDisplayMenu::enter()
{
    Serial.println(F("Entering temperature display menu"));
    this->unit = Unit; // Reset to default
}

void PickTemperatureDisplayMenu::onOffShortPress()
{
    Unit = this->unit; // Save the current unit
    deviceState = MainMenu;
    menu = &mainMenu;
    menu->enter(); // Call enter to reset the menu state
}

void PickTemperatureDisplayMenu::upPress()
{
    Serial.println(F("Up button pressed in temperature display menu"));
    this->unit = (this->unit == TemperatureUnit::Celsius) ? TemperatureUnit::Fahrenheit : TemperatureUnit::Celsius;
}

void PickTemperatureDisplayMenu::downPress()
{
    Serial.println(F("Down button pressed in temperature display menu"));
    this->unit = (this->unit == TemperatureUnit::Celsius) ? TemperatureUnit::Fahrenheit : TemperatureUnit::Celsius;
}

void PickTemperatureDisplayMenu::render()
{
    Serial.print(F("Current temperature display: "));
    Serial.println(this->unit);

    prepareScreen();
    display.setCursor(0, 30);
    display.print(F("Temp Unit: "));
    display.println((char) this->unit);
    display.display();
}

void SetTargetTempMenu::enter()
{
    Serial.println(F("Entering set target temperature menu"));
    this->targetTemp = TargetTemp; // Reset to the current target temperature
}

void SetTargetTempMenu::onOffShortPress()
{
    Serial.println(F("Exit set target temperature menu"));
    TargetTemp = this->targetTemp; // Save the current target temperature
    deviceState = MainMenu;
    menu = &mainMenu;
}

void SetTargetTempMenu::upPress()
{
    this->targetTemp = min(this->targetTemp + 0.5, 50); // Limit target temperature to a maximum of 50
}

void SetTargetTempMenu::downPress()
{
    this->targetTemp = max(this->targetTemp - 0.5, 0); // Limit target temperature to a minimum of 0
}

void SetTargetHumidityMenu::enter()
{
    Serial.println(F("Entering set target humidity menu"));
    this->targetHumidity = TargetHumidity; // Reset to the current target humidity
}

void SetTargetHumidityMenu::onOffShortPress()
{
    Serial.println(F("Exit set target humidity menu"));
    TargetHumidity = this->targetHumidity; // Save the current target humidity
    deviceState = MainMenu;
    menu = &mainMenu;
}

void SetTargetHumidityMenu::upPress()
{
    this->targetHumidity = min(this->targetHumidity + 1, 80); // Limit target humidity to a maximum of 80
}

void SetTargetHumidityMenu::downPress()
{
    this->targetHumidity = max(this->targetHumidity - 1, 0); // Limit target humidity to a minimum of 0
}

void SetTemperatureCalibrationMenu::enter()
{
    Serial.println(F("Entering set temperature calibration menu"));
    this->temperatureCalibration = TemperatureCalibration; // Reset to default calibration value
}

void SetTemperatureCalibrationMenu::onOffShortPress()
{
    Serial.println(F("Exit set temperature calibration menu"));
    TemperatureCalibration = this->temperatureCalibration; // Save the current temperature calibration
    deviceState = MainMenu;
    menu = &mainMenu;
    menu->enter(); // Call enter to reset the menu state
}

void SetTemperatureCalibrationMenu::upPress()
{
    this->temperatureCalibration += 0.1; // Increase temperature calibration by 0.1
}

void SetTemperatureCalibrationMenu::downPress()
{
    this->temperatureCalibration -= 0.1; // Decrease temperature calibration by 0.1
}

void SetHumidityCalibrationMenu::enter()
{
    Serial.println(F("Entering set humidity calibration menu"));
    this->humidityCalibration = HumidityCalibration; // Reset to default calibration value
}

void SetHumidityCalibrationMenu::onOffShortPress()
{
    Serial.println(F("Exit set humidity calibration menu"));
    HumidityCalibration = this->humidityCalibration; // Save the current humidity calibration
    deviceState = MainMenu;
    menu = &mainMenu;
    menu->enter(); // Call enter to reset the menu state
}

void SetHumidityCalibrationMenu::upPress()
{
    this->humidityCalibration += 0.1; // Increase humidity calibration by 0.1
}

void SetHumidityCalibrationMenu::downPress()
{
    this->humidityCalibration -= 0.1; // Decrease humidity calibration by 0.1
}

MainScreenMenu mainScreenMenu = MainScreenMenu();
MainSettingsMenu mainMenu = MainSettingsMenu();
PickTemperatureDisplayMenu pickTemperatureDisplayMenu = PickTemperatureDisplayMenu();
SetTargetTempMenu setTargetTempMenu = SetTargetTempMenu();
SetTargetHumidityMenu setTargetHumidityMenu = SetTargetHumidityMenu();
SetTemperatureCalibrationMenu setTemperatureCalibrationMenu = SetTemperatureCalibrationMenu();
SetHumidityCalibrationMenu setHumidityCalibrationMenu = SetHumidityCalibrationMenu();
