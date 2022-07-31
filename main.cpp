#include "mainwindow.h"
#include "../Executive.h"
#include <QApplication>
#include <QApplication>
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    
    MainWindow w;
    w.show();
    return a.exec();
}
