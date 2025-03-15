#include "sensorlabels.h"

SensorLabels::SensorLabels(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void SensorLabels::setupUI()
{
    tempLabel = new QLabel(this);
    humidityLabel = new QLabel(this);
    pressureLabel = new QLabel(this);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->addWidget(tempLabel);
    layout->addWidget(humidityLabel);
    layout->addWidget(pressureLabel);
}

void SensorLabels::updateLabels(const ClimateControl &climateControl, TemperatureConverter::Unit unit)
{
    float convertedTemp = TemperatureConverter::convert(climateControl.getTemperature(), unit);
    tempLabel->setText(QString("Температура: %1 %2")
                           .arg(convertedTemp)
                           .arg(TemperatureConverter::unitToString(unit)));
    humidityLabel->setText(climateControl.getHumidityText());
    pressureLabel->setText(climateControl.getPressureText());
}
