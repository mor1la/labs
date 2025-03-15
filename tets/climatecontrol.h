// climatecontrol.h
#ifndef CLIMATECONTROL_H
#define CLIMATECONTROL_H

#include <QString>

class ClimateControl {
public:
    ClimateControl()
        : temperature(0), humidity(0), pressure(0), isOn(false), airDirection("Вперед") {};


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
    QString getAirDirection() const;

    int getTemperature() const;
    int getHumidity() const;
    int getPressure() const;


    void setTemperature(float temperature);
    void setPressure(float pressure);
    void setHumidity(float humidity);
    void setAirDirection(const QString &direction);

    float calculateHumidityBasedOnTemperatureAndPressure();


private:
    int temperature;
    int humidity;
    int pressure;
    bool isOn;
    QString airDirection;
};

#endif // CLIMATECONTROL_H
