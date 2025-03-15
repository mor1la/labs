#include "mainwindow.h"
#include "strings.h"

MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), climateControl(), userSettings(FILENAME) {
    int tempUnitIndex = 0, pressureUnitIndex = 0, currentTemperature = 0, currentPressure = 0;

    if (!userSettings.loadSettings(tempUnitIndex, pressureUnitIndex, currentTemperature, currentPressure)) {
        currentTemperature = 0;
        currentPressure = 0;
        tempUnitIndex = 0;
        pressureUnitIndex = 0;
    }

    setupUI();
    setupConnections();
    climateControl.setTemperature(currentTemperature);
    climateControl.setPressure(currentPressure);

    unitSelector->setCurrentIndex(tempUnitIndex);
    pressureUnitSelector->setCurrentIndex(pressureUnitIndex);

    updateLabels();
}

MainWindow::~MainWindow() {
    userSettings.saveSettings(
        unitSelector->currentIndex(),
        pressureUnitSelector->currentIndex(),
        climateControl.getTemperature(),
        climateControl.getPressure()
        );
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *mainLayout = new QVBoxLayout(centralWidget);
    themeToggleButton = new QPushButton(Strings::switchThemeButton, this);

    mainLayout->addWidget(createTitle(), 0, Qt::AlignCenter);
    mainLayout->addWidget(createStatusGroup());
    mainLayout->addWidget(createControlsGroup());
    mainLayout->addWidget(themeToggleButton, 0, Qt::AlignCenter);

    setCentralWidget(centralWidget);
    resize(800, 600);
}

QLabel* MainWindow::createTitle() {
    QLabel *titleLabel = new QLabel(Strings::climateControlSystemTitle, this);
    titleLabel->setStyleSheet("font-size: 18px; font-weight: bold; text-align: center;");
    return titleLabel;
}

QGroupBox* MainWindow::createStatusGroup() {
    QGroupBox *statusGroup = new QGroupBox(Strings::systemControlGroup, this);
    QVBoxLayout *statusLayout = new QVBoxLayout(statusGroup);

    tempLabel = new QLabel(this);
    humidityLabel = new QLabel(this);
    pressureLabel = new QLabel(this);

    statusLayout->addWidget(tempLabel);
    statusLayout->addWidget(humidityLabel);
    statusLayout->addWidget(pressureLabel);

    return statusGroup;
}

QGroupBox* MainWindow::createControlsGroup() {
    QGroupBox *controlsGroup = new QGroupBox(Strings::systemControlGroupTitle, this);
    QGridLayout *controlsLayout = new QGridLayout(controlsGroup);

    toggleButton = new QPushButton(Strings::toggleButtonOn, this);
    controlsLayout->addWidget(toggleButton, 0, 0, 1, 2);

    increaseTempButton = new QPushButton(Strings::increaseTempButton, this);
    decreaseTempButton = new QPushButton(Strings::decreaseTempButton, this);
    increaseHumidityButton = new QPushButton(Strings::increaseHumidityButton, this);
    decreaseHumidityButton = new QPushButton(Strings::decreaseHumidityButton, this);
    increasePressureButton = new QPushButton(Strings::increasePressureButton, this);
    decreasePressureButton = new QPushButton(Strings::decreasePressureButton, this);

    controlsLayout->addWidget(increaseTempButton, 1, 0);
    controlsLayout->addWidget(decreaseTempButton, 1, 1);
    controlsLayout->addWidget(increaseHumidityButton, 2, 0);
    controlsLayout->addWidget(decreaseHumidityButton, 2, 1);
    controlsLayout->addWidget(increasePressureButton, 3, 0);
    controlsLayout->addWidget(decreasePressureButton, 3, 1);

    unitSelector = new QComboBox(this);
    unitSelector->addItems({Strings::celsius, Strings::fahrenheit, Strings::kelvin});
    controlsLayout->addWidget(new QLabel(Strings::tempUnitLabel), 4, 0);
    controlsLayout->addWidget(unitSelector, 4, 1);

    pressureUnitSelector = new QComboBox(this);
    pressureUnitSelector->addItems({"Па", "мм рт. ст."});
    controlsLayout->addWidget(new QLabel(Strings::pressureUnitLabel), 5, 0);
    controlsLayout->addWidget(pressureUnitSelector, 5, 1);

    inputTemperatureButton = new QPushButton(Strings::inputTemperatureButton, this);
    inputHumidityButton = new QPushButton(Strings::inputHumidityButton, this);
    inputPressureButton = new QPushButton(Strings::inputPressureButton, this);

    controlsLayout->addWidget(inputTemperatureButton, 6, 0, 1, 2);
    controlsLayout->addWidget(inputHumidityButton, 7, 0, 1, 2);
    controlsLayout->addWidget(inputPressureButton, 8, 0, 1, 2);

    directionSelector = new QComboBox(this);
    directionSelector->addItems({Strings::upDirection, Strings::downDirection, Strings::leftDirection, Strings::rightDirection});
    controlsLayout->addWidget(new QLabel(Strings::airDirectionLabelString), 9, 0);
    controlsLayout->addWidget(directionSelector, 9, 1);

    return controlsGroup;
}

void MainWindow::setupConnections() {
    connect(themeToggleButton, &QPushButton::clicked, &themeSwitcher, &ThemeSwitcher::toggleTheme);
    connect(toggleButton, &QPushButton::clicked, this, &MainWindow::toggleSystem);
    connect(increaseTempButton, &QPushButton::clicked, this, &MainWindow::increaseTemperature);
    connect(decreaseTempButton, &QPushButton::clicked, this, &MainWindow::decreaseTemperature);
    connect(increaseHumidityButton, &QPushButton::clicked, this, &MainWindow::increaseHumidity);
    connect(decreaseHumidityButton, &QPushButton::clicked, this, &MainWindow::decreaseHumidity);
    connect(increasePressureButton, &QPushButton::clicked, this, &MainWindow::increasePressure);
    connect(decreasePressureButton, &QPushButton::clicked, this, &MainWindow::decreasePressure);
    connect(unitSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateLabels);
    connect(pressureUnitSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateLabels);
    connect(inputTemperatureButton, &QPushButton::clicked, this, &MainWindow::openTemperatureInputDialog);
    connect(inputPressureButton, &QPushButton::clicked, this, &MainWindow::openPressureInputDialog);
    connect(inputHumidityButton, &QPushButton::clicked, this, &MainWindow::openHumidityInputDialog);
    connect(directionSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::changeAirDirection);
}

void MainWindow::toggleSystem() {
    climateControl.togglePower();
    toggleButton->setText(climateControl.isSystemOn() ? Strings::turnOffAirConditioner : Strings::turnOnAirConditioner);
}

void MainWindow::increaseTemperature() {
    climateControl.increaseTemperature();
    updateLabels();
}

void MainWindow::decreaseTemperature() {
    climateControl.decreaseTemperature();
    updateLabels();
}

void MainWindow::increaseHumidity() {
    climateControl.increaseHumidity();
    if (climateControl.getHumidity() > 100) {
        QMessageBox::warning(this, Strings::warningHumidityOver100, Strings::warningHumidityOver100);
        climateControl.setHumidity(100);
    }
    updateLabels();
}

void MainWindow::decreaseHumidity() {
    climateControl.decreaseHumidity();
    if (climateControl.getHumidity() < 0) {
        climateControl.setHumidity(0);
    }
    updateLabels();
}

void MainWindow::increasePressure() {
    climateControl.increasePressure();
    updateLabels();
}

void MainWindow::decreasePressure() {
    climateControl.decreasePressure();
    updateLabels();
}

void MainWindow::changeAirDirection() {
    int directionIndex = directionSelector->currentIndex();
    QString direction = directionSelector->itemText(directionIndex);
    climateControl.setAirDirection(direction);
    QMessageBox::information(this, Strings::airDirectionLabel, Strings::directionChosenMessage + direction);
}

void MainWindow::openTemperatureInputDialog() {
    TemperatureInputDialog inputDialog(this);
    if (inputDialog.exec() == QDialog::Accepted) {
        climateControl.setTemperature(inputDialog.getValue());
        updateLabels();
    }
}

void MainWindow::openPressureInputDialog() {
    PressureInputDialog inputDialog(this);
    if (inputDialog.exec() == QDialog::Accepted) {
        climateControl.setPressure(inputDialog.getValue());
        updateLabels();
    }
}

void MainWindow::openHumidityInputDialog() {
    HumidityInputDialog inputDialog(this);
    if (inputDialog.exec() == QDialog::Accepted) {
        float humidity = inputDialog.getValue();

        if (humidity > 100) {
            QMessageBox::warning(this, Strings::warningHumidityOver100, Strings::warningHumidityOver100);
        } else if (humidity < 0) {
            QMessageBox::warning(this, Strings::warningHumidityUnder0, Strings::warningHumidityUnder0);
        } else {
            climateControl.setHumidity(humidity);
            updateLabels();
        }
    }
}

void MainWindow::updateLabels() {
    IConverter* tempConverter = new TemperatureConverter();
    int tempUnit = unitSelector->currentIndex();
    float convertedTemp = tempConverter->convert(climateControl.getTemperature(), tempUnit);
    tempLabel->setText(QString("Температура: %1 %2")
                           .arg(convertedTemp)
                           .arg(tempConverter->unitToString(tempUnit)));
    humidityLabel->setText(climateControl.getHumidityText());

    IConverter* pressureConverter = new PressureConverter();
    int pressureUnit = pressureUnitSelector->currentIndex();
    float pressure = climateControl.getPressure();
    float convertedPressure = pressureConverter->convert(pressure, pressureUnit);
    QString pressureText = QString("Давление: %1 %2")
                               .arg(convertedPressure)
                               .arg(pressureConverter->unitToString(pressureUnit));
    pressureLabel->setText(pressureText);

    delete tempConverter;
    delete pressureConverter;
}

void MainWindow::openInputDialog() {
    bool ok;
    QString text = QInputDialog::getText(this, Strings::inputDataTitle,
                                         Strings::inputDataMessage, QLineEdit::Normal,
                                         QString(), &ok);

    if (ok && !text.isEmpty()) {
        QMessageBox::information(this, Strings::inputDataTitle, Strings::enteredDataMessage + text);
    } else if (!ok) {
        QMessageBox::information(this, Strings::inputDataTitle, Strings::cancelMessage);
    } else {
        QMessageBox::warning(this, Strings::inputDataTitle, Strings::errorMessage);
    }
}
