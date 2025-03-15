#ifndef ICONVERTER_H
#define ICONVERTER_H

#include <QString>

class IConverter {
public:
    virtual ~IConverter() = default;

    virtual float convert(float value, int unit) const = 0;

    virtual QString unitToString(int unit) const = 0;
};

#endif // ICONVERTER_H
