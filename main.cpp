#include <QApplication>

#include "mainwin.h"

int main(int ac,char** av) {
	QApplication app(ac,av);

	MWin win;
	win.show();

	return app.exec();
}
