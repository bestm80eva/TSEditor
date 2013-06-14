#ifndef MLABEL_H
#define MLABEL_H

#include <QLabel>

#define	ML_PALEDIT	1
#define	ML_TILEPAL	2
#define	ML_TILEDIT	3
#define	ML_BIGVIEW	4

class MLabel : public QLabel {
	Q_OBJECT
	public:
		MLabel(QWidget* p=NULL);
		int type;
		int colidx;
	signals:
		void colChanged(int);
		void tileChanged(int);
	private:
		void drawEditBox(int,int,int,QColor,QPainter*);
	protected:
		void paintEvent(QPaintEvent*);
		void mousePressEvent(QMouseEvent*);
		void mouseMoveEvent(QMouseEvent*);
};

#endif // MLABEL_H
