#include "climatecontrol.h"

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

QString ClimateControl::getPressureText() const {
    return QString("Давление: %1 Па").arg(pressure);
}

QString ClimateControl::getHumidityText() const {
    return QString("Влажность: %1%").arg(humidity);
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

// Установить влажность вручную, если нужно
void ClimateControl::setHumidity(float humidity) {
    this->humidity = humidity;
}


// Функция расчета влажности
float ClimateControl::calculateHumidityBasedOnTemperatureAndPressure() {
    // Примерная формула для расчета влажности (здесь вставить нужную)
    const float baseHumidity = 50.0;
    const float temperatureFactor = 0.2;
    const float pressureFactor = 0.05;

    float calculatedHumidity = baseHumidity + (temperatureFactor * temperature) - (pressureFactor * pressure);

    if (calculatedHumidity < 0) calculatedHumidity = 0;
    if (calculatedHumidity > 100) calculatedHumidity = 100;

    return calculatedHumidity;
}

void ClimateControl::setAirDirection(const QString &direction) {
    if (direction == "Вверх" || direction == "Вниз" || direction == "Вперед" || direction == "Назад") {
        airDirection = direction;
    }
}

QString ClimateControl::getAirDirection() const {
    return airDirection;
}
