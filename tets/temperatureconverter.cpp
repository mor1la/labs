#include "TemperatureConverter.h"

float TemperatureConverter::convert(float tempCelsius, int unit) const {
    switch (unit) {
    case Celsius: return tempCelsius;
    case Fahrenheit: return tempCelsius * 9.0 / 5.0 + 32;
    case Kelvin: return tempCelsius + 273.15;
    default: return tempCelsius;
    }
}

QString TemperatureConverter::unitToString(int unit) const {
    switch (unit) {
    case Celsius: return "°C";
    case Fahrenheit: return "°F";
    case Kelvin: return "K";
    default: return "";
    }
}
