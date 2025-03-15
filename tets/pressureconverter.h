#ifndef PRESSURECONVERTER_H
#define PRESSURECONVERTER_H

#include "iconverter.h"

class PressureConverter : public IConverter {
public:
    enum Unit {
        Pascals,
        MmHg
    };

    float convert(float pressurePascals, int unit) const override;

    QString unitToString(int unit) const override;
};

#endif // PRESSURECONVERTER_H
