// humidityinputdialog.cpp
#include "humidityinputdialog.h"



float HumidityInputDialog::getValue() const
{
    return inputField->text().toFloat();
}
