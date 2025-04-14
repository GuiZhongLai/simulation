#include "simulation.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    simulation w;
    w.showMaximized();
    return a.exec();
}
