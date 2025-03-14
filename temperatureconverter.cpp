#include "TemperatureConverter.h"

float TemperatureConverter::convert(float tempCelsius, Unit unit) {
    switch (unit) {
    case Celsius:
        return tempCelsius;
    case Fahrenheit:
        return tempCelsius * 9.0 / 5.0 + 32;
    case Kelvin:
        return tempCelsius + 273;
    default:
        return tempCelsius;
    }
}

QString TemperatureConverter::unitToString(Unit unit) {
    switch (unit) {
    case Celsius:
        return "Цельсий";
    case Fahrenheit:
        return "Фаренгейт";
    case Kelvin:
        return "Кельвин";
    default:
        return "Неизвестная единица";
    }
}
