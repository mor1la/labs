#include "PressureConverter.h"

float PressureConverter::convert(float pressurePascals, int unit) const {
    switch (unit) {
    case Pascals: return pressurePascals;
    case MmHg: return pressurePascals / 133.322;
    default: return pressurePascals;
    }
}

QString PressureConverter::unitToString(int unit) const {
    switch (unit) {
    case Pascals: return "Па";
    case MmHg: return "мм рт. ст.";
    default: return "";
    }
}
