#ifndef _MAINWIN_H
#define	_MAINWIN_H

#include <QMainWindow>
#include "mlabel.h"

#define	BLK_CHA		1

class MWin : public QMainWindow {
	Q_OBJECT
	public:
		MWin(QWidget* p=NULL);
	private:
		int flag;
		MLabel bigview;
		void updatePal();
		void saveTiles(QString);
		void savePal(QString);
		void openTiles(QString);
		void openPal(QString);
	private slots:
		void changeCol(int);
		void colChanged();
		void palChange();
		void tilChange(int);
		void pickTile();

		void openPrj();
		void savePrj();
};

#endif
