#ifndef UNITSELECTOR_H
#define UNITSELECTOR_H

#include <QWidget>
#include <QComboBox>

class UnitSelector : public QWidget
{
    Q_OBJECT

public:
    explicit UnitSelector(QWidget *parent = nullptr);

    int getSelectedUnit() const;

signals:
    void unitChanged(int index);

private:
    QComboBox *unitSelector;

    void setupUI();
};

#endif // UNITSELECTOR_H
