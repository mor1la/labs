#ifndef CONTROLBUTTONS_H
#define CONTROLBUTTONS_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>

class ControlButtons : public QWidget
{
    Q_OBJECT

public:
    explicit ControlButtons(QWidget *parent = nullptr);

signals:
    void increaseTemperature();
    void decreaseTemperature();
    void increaseHumidity();
    void decreaseHumidity();
    void increasePressure();
    void decreasePressure();

private:
    QPushButton *increaseTempButton;
    QPushButton *decreaseTempButton;
    QPushButton *increaseHumidityButton;
    QPushButton *decreaseHumidityButton;
    QPushButton *increasePressureButton;
    QPushButton *decreasePressureButton;

    void setupUI();
    void setupConnections();
};

#endif // CONTROLBUTTONS_H
