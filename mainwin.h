#ifndef _MAINWIN_H
#define	_MAINWIN_H

#include <QMainWindow>
#include <QDialog>
#include "mlabel.h"

#define	BLK_CHA		1
#define	PICK_CP		(1<<1)		// copy selected tile to current instead

class TMEdit : public QDialog {
	Q_OBJECT
	public:
		TMEdit(QWidget* p=NULL);
	private:
		QString tmPath;
	private slots:
		void tileRowUp();
		void tileRowDn();
		void openTMap();
		void saveTMap();
	protected:
		void keyPressEvent(QKeyEvent*);
};

class MWin : public QMainWindow {
	Q_OBJECT
	public:
		MWin(QWidget* p=NULL);
	private:
		int flag;
		MLabel bigview;
		TMEdit* tedit;
		void updatePal();
	private slots:
		void changeCol(int);
		void colChanged();
		void palChange();
		void tilChange(int);

		void pickTile();
		void pickCopy();
		void picked(int);

		void flipHor();
		void flipVer();
		void clearTile();

		void saveTiles();
		void savePal();
		void openTiles();
		void openPal();
};

#endif
