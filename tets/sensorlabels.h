#ifndef SENSORLABELS_H
#define SENSORLABELS_H

#include <QWidget>
#include <QLabel>
#include <QVBoxLayout>
#include "climatecontrol.h"
#include "temperatureconverter.h"

class SensorLabels : public QWidget
{
    Q_OBJECT

public:
    explicit SensorLabels(QWidget *parent = nullptr);

    void updateLabels(const ClimateControl &climateControl, TemperatureConverter::Unit unit);

private:
    QLabel *tempLabel;
    QLabel *humidityLabel;
    QLabel *pressureLabel;

    void setupUI();
};

#endif // SENSORLABELS_H
