#include "mainwindow.h"

#include <QVBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QMessageBox>
#include <QInputDialog>


MainWindow::MainWindow(QWidget *parent) : QMainWindow(parent), climateControl() {
    setupUI();
    setupConnections();
    updateLabels();
}

MainWindow::~MainWindow() {}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    QVBoxLayout *layout = new QVBoxLayout(centralWidget);

    tempLabel = new QLabel(this);
    humidityLabel = new QLabel(this);
    pressureLabel = new QLabel(this);
    toggleButton = new QPushButton("Включить кондиционер", this);

    increaseTempButton = new QPushButton("+ Температура", this);
    decreaseTempButton = new QPushButton("- Температура", this);
    increaseHumidityButton = new QPushButton("+ Влажность", this);
    decreaseHumidityButton = new QPushButton("- Влажность", this);
    increasePressureButton = new QPushButton("+ Давление", this);
    decreasePressureButton = new QPushButton("- Давление", this);

    unitSelector = new QComboBox(this);
    unitSelector->addItems({"Цельсий", "Фаренгейт", "Кельвин"});

    pressureUnitSelector = new QComboBox(this);
    pressureUnitSelector->addItems({"Па", "мм рт. ст."}); // Добавляем выбор единиц давления

    inputTemperatureButton = new QPushButton("Ввести температуру", this);
    inputPressureButton = new QPushButton("Ввести давление", this);
    inputHumidityButton = new QPushButton("Ввести влажность", this);

    layout->addWidget(tempLabel);
    layout->addWidget(humidityLabel);
    layout->addWidget(pressureLabel);
    layout->addWidget(toggleButton);
    layout->addWidget(increaseTempButton);
    layout->addWidget(decreaseTempButton);
    layout->addWidget(increaseHumidityButton);
    layout->addWidget(decreaseHumidityButton);
    layout->addWidget(increasePressureButton);
    layout->addWidget(decreasePressureButton);
    layout->addWidget(unitSelector);
    layout->addWidget(pressureUnitSelector);
    layout->addWidget(inputTemperatureButton);
    layout->addWidget(inputPressureButton);
    layout->addWidget(inputHumidityButton);

    setCentralWidget(centralWidget);
    resize(1024, 768);
}

void MainWindow::setupConnections() {
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
}

void MainWindow::toggleSystem() {
    climateControl.togglePower();
    toggleButton->setText(climateControl.isSystemOn() ? "Выключить кондиционер" : "Включить кондиционер");
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
    updateLabels();
}

void MainWindow::decreaseHumidity() {
    climateControl.decreaseHumidity();
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
        climateControl.setHumidity(inputDialog.getValue());
        updateLabels();
    }
}

void MainWindow::updateLabels() {
    TemperatureConverter::Unit currentUnit = static_cast<TemperatureConverter::Unit>(unitSelector->currentIndex());
    float convertedTemp = TemperatureConverter::convert(climateControl.getTemperature(), currentUnit);
    tempLabel->setText(QString("Температура: %1 %2")
                           .arg(convertedTemp)
                           .arg(TemperatureConverter::unitToString(currentUnit)));


    humidityLabel->setText(climateControl.getHumidityText());

    int pressureUnit = pressureUnitSelector->currentIndex();
    float pressure = climateControl.getPressure();
    QString pressureText;

    if (pressureUnit == 0) { // Па
        pressureText = QString("Давление: %1 Па").arg(pressure);
    } else { // мм рт. ст.
        pressureText = QString("Давление: %1 мм рт. ст.").arg(pressure / 133.322); // 1 мм рт. ст. = 133.322 Па
    }

    pressureLabel->setText(pressureText);
}

void MainWindow::openInputDialog() {
    bool ok;
    QString text = QInputDialog::getText(this, tr("Ввод данных"),
                                         tr("Введите значение:"), QLineEdit::Normal,
                                         QString(), &ok);

    if (ok && !text.isEmpty()) {
        QMessageBox::information(this, tr("Введенные данные"), tr("Вы ввели: %1").arg(text));
    } else if (!ok) {
        QMessageBox::information(this, tr("Отменено"), tr("Ввод был отменен"));
    } else {
        QMessageBox::warning(this, tr("Ошибка"), tr("Ничего не введено"));
    }
}
