#ifndef TEMPERATURECONVERTER_H
#define TEMPERATURECONVERTER_H

#include <QString>

class TemperatureConverter {
public:
    enum Unit {
        Celsius,
        Fahrenheit,
        Kelvin
    };

    static float convert(float tempCelsius, Unit unit);
    static QString unitToString(Unit unit);
};

#endif // TEMPERATURECONVERTER_H
