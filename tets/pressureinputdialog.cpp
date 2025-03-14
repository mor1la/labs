// pressureinputdialog.cpp
#include "pressureinputdialog.h"


float PressureInputDialog::getValue() const
{
    return inputField->text().toFloat();
}
