#ifndef THEMESWITCHER_H
#define THEMESWITCHER_H

#include <QObject>
#include <QApplication>

class ThemeSwitcher : public QObject {
    Q_OBJECT
public:
    explicit ThemeSwitcher(QObject *parent = nullptr);
    void toggleTheme();

private:
    bool darkMode;
    void applyDarkTheme();
    void applyLightTheme();
};

#endif // THEMESWITCHER_H
