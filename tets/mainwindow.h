#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QVBoxLayout>
#include <QComboBox>
#include <QMessageBox>
#include <QInputDialog>
#include <QGroupBox>
#include <QMainWindow>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include "climatecontrol.h"
#include "temperatureinputdialog.h"
#include "pressureinputdialog.h"
#include "humidityinputdialog.h"
#include "temperatureconverter.h"
#include "themeswitcher.h"
#include "usersettings.h"
#include "strings.qm"
#include "pressureconverter.h"


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
    void changeAirDirection();
    void updateLabels();


    void openTemperatureInputDialog();
    void openPressureInputDialog();
    void openHumidityInputDialog();
    void openInputDialog();

private:
    void setupUI();
    QLabel* createTitle();
    QGroupBox* createStatusGroup();
    QGroupBox* createControlsGroup();
    QPushButton* createThemeToggle();
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
    QComboBox *directionSelector;
    QComboBox *airDirectionSelector;

    QPushButton *inputTemperatureButton;
    QPushButton *inputHumidityButton;
    QPushButton *inputPressureButton;

    QPushButton *themeToggleButton;
    QLabel *systemStatusLabel;
    ThemeSwitcher themeSwitcher;
    ClimateControl climateControl;
    UserSettings userSettings;

};

#endif // MAINWINDOW_H
