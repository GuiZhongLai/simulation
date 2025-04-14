#pragma once

#include <QtWidgets/QMainWindow>
#include "ui_simulation.h"

class simulation : public QMainWindow
{
    Q_OBJECT

public:
    simulation(QWidget *parent = Q_NULLPTR);

private:
    Ui::simulationClass ui;

Q_SIGNALS:
	void signalValueChangedX(double v);
	void signalValueChangedY(double v);
	void signalValueChangedZ(double v);
};
