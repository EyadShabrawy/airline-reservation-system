#include "AirlineSystem.h"
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    AirlineSystem w;
    w.show();
    return a.exec();
}
