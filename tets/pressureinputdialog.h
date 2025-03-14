#ifndef PRESSUREINPUTDIALOG_H
#define PRESSUREINPUTDIALOG_H

#include "inputdialog.h"

class PressureInputDialog : public InputDialog
{
    Q_OBJECT

public:
    explicit PressureInputDialog(QWidget *parent = nullptr)
        : InputDialog(parent, "Давление", "Введите давление (Па)", 0, 200000) {}
    float getValue() const;
};

#endif // PRESSUREINPUTDIALOG_H
