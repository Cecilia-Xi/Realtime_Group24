#include "Executive.h"
#include <iostream>
#include <QtCore/QDebug>
#include <QtCore/QFile>
#include <QtWidgets/QApplication>

int main(int argc, char *argv[])
{	
	QApplication *qapp = new QApplication(argc, argv);
	
	

	Executive e1;
	e1.start()

	return 0;
}
