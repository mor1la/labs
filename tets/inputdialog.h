#ifndef INPUTDIALOG_H
#define INPUTDIALOG_H

#include <QDialog>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>
#include <QLabel>
#include <QValidator>

class InputDialog : public QDialog
{
    Q_OBJECT

public:
    explicit InputDialog(QWidget *parent = nullptr, const QString &title = "", const QString &label = "",
                         float minValue = -9999, float maxValue = 9999);

    float getValue() const;

protected:
    QLineEdit *inputField;
    float minValue;
    float maxValue;
};

#endif // INPUTDIALOG_H
