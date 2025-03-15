#include "unitselector.h"

UnitSelector::UnitSelector(QWidget *parent) : QWidget(parent)
{
    setupUI();
}

void UnitSelector::setupUI()
{
    unitSelector = new QComboBox(this);
    unitSelector->addItems({"Цельсий", "Фаренгейт", "Кельвин"});

    connect(unitSelector, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &UnitSelector::unitChanged);
}

int UnitSelector::getSelectedUnit() const
{
    return unitSelector->currentIndex();
}
