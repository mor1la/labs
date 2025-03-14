// climatecontrol.h
#ifndef CLIMATECONTROL_H
#define CLIMATECONTROL_H

#include <QString>

class ClimateControl {
public:
    ClimateControl();

    void togglePower();
    bool isSystemOn() const;

    void increaseTemperature();
    void decreaseTemperature();
    void increaseHumidity();
    void decreaseHumidity();
    void increasePressure();
    void decreasePressure();

    QString getTemperatureText() const;
    QString getHumidityText() const;
    QString getPressureText() const;

    int getTemperature() const;
    int getHumidity() const;
    int getPressure() const;

    void setTemperature(float temperature);
    void setPressure(float pressure);
    void setHumidity(float humidity);

private:
    int temperature;
    int humidity;
    int pressure;
    bool isOn;
};

#endif // CLIMATECONTROL_H
