#ifndef TEMPERATUREINPUTDIALOG_H
#define TEMPERATUREINPUTDIALOG_H

#include "inputdialog.h"

class TemperatureInputDialog : public InputDialog
{
    Q_OBJECT

public:
    explicit TemperatureInputDialog(QWidget *parent = nullptr)
        : InputDialog(parent, "Температура", "Введите температуру", -273.15, 1000) {}

    float getValue() const;
};

#endif // TEMPERATUREINPUTDIALOG_H
