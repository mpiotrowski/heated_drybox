#include <Fonts/FreeMono9pt7b.h>

#include "screen.h"
#include "drybox.h"

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void drawLogo()
{
    display.clearDisplay();
    display.drawBitmap(0, 0, Logo, SCREEN_WIDTH, SCREEN_HEIGHT, 1);
    display.display();
}

void prepareScreen()
{
    display.clearDisplay();
    display.setFont(NULL);
    display.setTextColor(SSD1306_WHITE);
    display.setTextSize(1);
    display.setCursor(0, 3);

    char targetTemp[7];
    formatTemperature(TargetTemp, true, targetTemp, sizeof(targetTemp));
    char targetHum[6];
    formatHumidity(TargetHumidity, targetHum, sizeof(targetHum));
    display.print(targetTemp);
    display.print(' ');
    display.print(targetHum);

    display.drawLine(0, 13, 126, 13, SSD1306_WHITE);

    drawheaterOn();
}

// str output is as least of length 7
void formatTemperature(float Temperature, bool round, char *str, size_t str_len)
{
    memset(str, ' ', str_len);

    float temp = 0.0;
    temp = min(Temperature + TemperatureCalibration, 70.); // Apply temperature calibration and limit to 70
    if (Unit == TemperatureUnit::Fahrenheit)
    {
        temp = temp * 9.0 / 5.0 + 32.0; // Convert Celsius to Fahrenheit
    }
    if(round)
    {
        temp = roundf(temp);
    }

    // Move the first digit to the right if temperature is less than 100
    int offset = (offset < 100) ? 4 : 5;
    dtostrf(temp, 2, 1, str);
    str[offset] = Unit;
    str[offset + 1] = '\0';
}

void formatHumidity(float Humidity, char *str, size_t str_len)
{
    memset(str, ' ', str_len);
    str[5] = '\0'; // Null-terminate the string

    if (Humidity < 10)
        dtostrf(Humidity, 2, 1, str + 1);
    else
        dtostrf(Humidity, 3, 1, str);
    str[4] = '%'; // Add percentage sign
}

void drawheaterOn()
{
    display.fillCircle(114, 6, 5, SSD1306_WHITE);
    if (!heaterOn)
    {
        display.fillCircle(114, 6, 4, SSD1306_BLACK);
    }
}

void drawTemperature()
{
}

void drawHumidity()
{
}

void drawTargetTemperature()
{
}
