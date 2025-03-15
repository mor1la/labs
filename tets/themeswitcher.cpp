#include "themeswitcher.h"

ThemeSwitcher::ThemeSwitcher(QObject *parent) : QObject(parent), darkMode(false) {}

void ThemeSwitcher::toggleTheme() {
    darkMode = !darkMode;
    if (darkMode) {
        applyDarkTheme();
    } else {
        applyLightTheme();
    }
}


void ThemeSwitcher::applyDarkTheme() {
    qApp->setStyleSheet(""
                        "QWidget { background-color: #121212; color: #ffffff; }"
                        "QPushButton { background-color: #333333; color: #ffffff; border: 1px solid #555555; padding: 5px; }"
                        "QPushButton:hover { background-color: #444444; }"  // добавляем эффект при наведении
                        "QPushButton:pressed { background-color: #555555; }"  // добавляем эффект при нажатии
                        "QLabel { color: #ffffff; margin: 2px; }"
                        "QGroupBox { border: 1px solid #555555; color: #ffffff; padding: 10px; margin-top: 1px; }"
                        "");
}


void ThemeSwitcher::applyLightTheme() {
    qApp->setStyleSheet("");
}
