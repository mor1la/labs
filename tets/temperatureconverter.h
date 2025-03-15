#ifndef TEMPERATURECONVERTER_H
#define TEMPERATURECONVERTER_H

#include "iconverter.h"

class TemperatureConverter : public IConverter {
public:
    enum Unit {
        Celsius,
        Fahrenheit,
        Kelvin
    };

    // Реализация метода конвертации температуры
    float convert(float tempCelsius, int unit) const override;

    // Реализация метода для преобразования единицы в строку
    QString unitToString(int unit) const override;
};

#endif // TEMPERATURECONVERTER_H
