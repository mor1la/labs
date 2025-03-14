// temperatureinputdialog.cpp
#include "temperatureinputdialog.h"


float TemperatureInputDialog::getValue() const
{
    return inputField->text().toFloat();
}

