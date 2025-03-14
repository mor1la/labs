#ifndef HUMIDITYINPUTDIALOG_H
#define HUMIDITYINPUTDIALOG_H

#include "inputdialog.h"

class HumidityInputDialog : public InputDialog
{
    Q_OBJECT

public:
    explicit HumidityInputDialog(QWidget *parent = nullptr)
        : InputDialog(parent, "Влажность", "Введите влажность (0-100%)", 0, 100) {}
    float getValue() const;
};

#endif // HUMIDITYINPUTDIALOG_H
