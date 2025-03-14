#include "inputdialog.h"

InputDialog::InputDialog(QWidget *parent, const QString &title, const QString &label,
                         float minValue, float maxValue)
    : QDialog(parent), minValue(minValue), maxValue(maxValue)
{
    setWindowTitle(title);
    QVBoxLayout *layout = new QVBoxLayout(this);

    QLabel *prompt = new QLabel(label, this);
    inputField = new QLineEdit(this);
    QPushButton *okButton = new QPushButton("OK", this);
    QPushButton *cancelButton = new QPushButton("Отмена", this);

    layout->addWidget(prompt);
    layout->addWidget(inputField);
    layout->addWidget(okButton);
    layout->addWidget(cancelButton);

    connect(okButton, &QPushButton::clicked, this, &QDialog::accept);
    connect(cancelButton, &QPushButton::clicked, this, &QDialog::reject);
}

float InputDialog::getValue() const
{
    float value = inputField->text().toFloat();
    return qBound(minValue, value, maxValue); // Ограничение значений
}
