#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QPushButton>
#include <QComboBox>
#include <QLabel>
#include <QMessageBox>
#include "climatecontrol.h"
#include "temperatureinputdialog.h"
#include "pressureinputdialog.h"
#include "humidityinputdialog.h"
#include "temperatureconverter.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void toggleSystem();
    void increaseTemperature();
    void decreaseTemperature();
    void increaseHumidity();
    void decreaseHumidity();
    void increasePressure();
    void decreasePressure();
    void updateLabels();

    void openTemperatureInputDialog();
    void openPressureInputDialog();
    void openHumidityInputDialog();
    void openInputDialog();

private:
    void setupUI();
    void setupConnections();

    QLabel *tempLabel;
    QLabel *humidityLabel;
    QLabel *pressureLabel;
    QPushButton *toggleButton;
    QPushButton *increaseTempButton;
    QPushButton *decreaseTempButton;
    QPushButton *increaseHumidityButton;
    QPushButton *decreaseHumidityButton;
    QPushButton *increasePressureButton;
    QPushButton *decreasePressureButton;
    QComboBox *unitSelector;
    QComboBox *pressureUnitSelector;
    QPushButton *inputTemperatureButton;
    QPushButton *inputPressureButton;
    QPushButton *inputHumidityButton;

    ClimateControl climateControl;
};

#endif // MAINWINDOW_H
