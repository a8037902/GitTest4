#include <QtWidgets/QApplication>
#include "../libScreenShot/libScreenShot.h"

int main(int argc, char *argv[])
{
	QApplication a(argc, argv);
	QSimpleScreenShot::instance()->startScreenShot();
	return a.exec();
}