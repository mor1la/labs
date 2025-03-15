#include "usersettings.h"

void UserSettings::saveSettings(int temperatureUnitIndex, int pressureUnitIndex, int currentTemperature, int currentPressure){
    QFile file(settingsFileName);
    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::warning(nullptr, "Ошибка", "Не удалось сохранить настройки.");
        return;
    }

    QXmlStreamWriter xmlWriter(&file);
    xmlWriter.setAutoFormatting(true);
    xmlWriter.writeStartDocument();
    xmlWriter.writeStartElement("Settings");

    // Запись единиц измерения
    xmlWriter.writeTextElement("TemperatureUnit", QString::number(temperatureUnitIndex));
    xmlWriter.writeTextElement("PressureUnit", QString::number(pressureUnitIndex));

    // Запись текущих значений
    xmlWriter.writeTextElement("CurrentTemperature", QString::number(currentTemperature));
    xmlWriter.writeTextElement("CurrentPressure", QString::number(currentPressure));

    xmlWriter.writeEndElement(); // Закрываем <Settings>
    xmlWriter.writeEndDocument();

    file.close();
}


bool UserSettings::loadSettings(int &temperatureUnitIndex, int &pressureUnitIndex, int &currentTemperature, int &currentPressure) {
    QFile file(settingsFileName);
    if (!file.open(QIODevice::ReadOnly)) {
        return false; // Файл не найден или не может быть открыт
    }

    QXmlStreamReader xmlReader(&file);
    while (!xmlReader.atEnd() && !xmlReader.hasError()) {
        QXmlStreamReader::TokenType token = xmlReader.readNext();
        if (token == QXmlStreamReader::StartElement) {
            if (xmlReader.name() == "TemperatureUnit") {
                temperatureUnitIndex = xmlReader.readElementText().toInt();
            } else if (xmlReader.name() == "PressureUnit") {
                pressureUnitIndex = xmlReader.readElementText().toInt();
            } else if (xmlReader.name() == "CurrentTemperature") {
                currentTemperature = xmlReader.readElementText().toInt();
            } else if (xmlReader.name() == "CurrentPressure") {
                currentPressure = xmlReader.readElementText().toInt();
            }
        }
    }

    file.close();
    return !xmlReader.hasError(); // Проверка на ошибки при разборе XML
}

