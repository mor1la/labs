#include "climatecontrol.h"

ClimateControl::ClimateControl() : temperature(0), humidity(0), pressure(0), isOn(false) {}

void ClimateControl::togglePower() {
    isOn = !isOn;
}

bool ClimateControl::isSystemOn() const {
    return isOn;
}

void ClimateControl::increaseTemperature() {
    if (isOn) {
        temperature++;
    }
}

void ClimateControl::decreaseTemperature() {
    if (isOn) {
        temperature--;
    }
}

void ClimateControl::increaseHumidity() {
    if (isOn && humidity < 100) {
        humidity++;
    }
}

void ClimateControl::decreaseHumidity() {
    if (isOn && humidity > 0) {
        humidity--;
    }
}

void ClimateControl::increasePressure() {
    if (isOn) {
        pressure += 10;
    }
}

void ClimateControl::decreasePressure() {
    if (isOn && pressure > 0) {
        pressure -= 10;
    }
}

QString ClimateControl::getTemperatureText() const {
    return QString("Температура: %1°C").arg(temperature);
}

QString ClimateControl::getHumidityText() const {
    return QString("Влажность: %1%").arg(humidity);
}

QString ClimateControl::getPressureText() const {
    return QString("Давление: %1 Па").arg(pressure);
}
int ClimateControl::getTemperature() const {
    return temperature;
}

int ClimateControl::getHumidity() const {
    return humidity;
}

int ClimateControl::getPressure() const {
    return pressure;
}

void ClimateControl::setTemperature(float temperature) {
    this->temperature = temperature;
}

void ClimateControl::setPressure(float pressure) {
    this->pressure = pressure;
}

void ClimateControl::setHumidity(float humidity) {
    this->humidity = humidity;
}
