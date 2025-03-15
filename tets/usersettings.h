#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QString>
#include <QFile>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMessageBox>
#define FILENAME "settings.xml"


class UserSettings {
public:
    UserSettings(const QString& fileName) : settingsFileName(FILENAME) {}

    void saveSettings(int temperatureUnitIndex, int pressureUnitIndex, int currentTemperature, int currentPressure) ;
    bool loadSettings(int &temperatureUnitIndex, int &pressureUnitIndex, int &currentTemperature, int &currentPressure);

private:
    QString settingsFileName;
};

#endif // USERSETTINGS_H
