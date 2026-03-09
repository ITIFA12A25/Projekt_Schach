#include "hauptmenue.h"
#include "controller.h"
#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    //startet Controller
    Controller controller;
    return a.exec();
}
