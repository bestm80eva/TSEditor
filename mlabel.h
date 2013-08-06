#ifndef MLABEL_H
#define MLABEL_H

#include <QLabel>

#define	ML_PALEDIT	1
#define	ML_TILEPAL	2
#define	ML_TILEDIT	3
#define	ML_BIGVIEW	4
#define	ML_TILEROW	5
#define	ML_TILEMAP	6
#define	ML_TILESHOW	7
#define	ML_VIEW		8

class MLabel : public QLabel {
	Q_OBJECT
	public:
		MLabel(QWidget* p=NULL);
		int type;
		int colidx;
		int row;
		int xpos;
		int ypos;
	signals:
		void colChanged(int);
		void tileChanged(int);
	private:
		void drawEditBox(int,int,int,QColor,QPainter*);
		void drawTile(int,int,int,int,QPainter*);
	protected:
		void paintEvent(QPaintEvent*);
		void mousePressEvent(QMouseEvent*);
		void mouseMoveEvent(QMouseEvent*);
};

#endif // MLABEL_H
