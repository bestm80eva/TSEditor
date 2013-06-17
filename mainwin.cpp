#include "mainwin.h"
#include "mlabel.h"
#include "ui_mainwin.h"
#include "ui_tmedit.h"

#include <QtGui>

struct Tile {
	int pal;
	unsigned char data[64];
};

struct palItem {
	int r;
	int g;
	int b;
	QColor col;
};

unsigned char tslCoLevs[32] = {
	0,11,21,32,42,53,64,74,
	85,95,106,117,127,138,148,159,
	170,180,191,201,212,223,233,244,
	255,255,255,255,255,255,255,255
};

int tileMap[2][4096];

Tile tiles[4096];
Ui::MWin ui;
Ui::TMEdit teui;
palItem pal[256];

MWin::MWin(QWidget *p):QMainWindow(p) {

	tedit = new TMEdit(this);

	ui.setupUi(this);

	ui.paledit->type = ML_PALEDIT;
	ui.paledit->colidx = 0;
	ui.tilePalGrid->type = ML_TILEPAL;
	ui.tiledit->type = ML_TILEDIT;
	ui.colwid->setEnabled(false);

	bigview.type = ML_BIGVIEW;
	bigview.setWindowModality(Qt::ApplicationModal);
	bigview.setFixedSize(512,512);

	for (int i = 0; i < 16; i++) {
		pal[i << 4].r = -1;
		pal[i << 4].g = -1;
		pal[i << 4].b = -1;
		pal[i << 4].col = QColor(100,100,100);
	}

	connect(ui.paledit,SIGNAL(colChanged(int)),this,SLOT(changeCol(int)));
	connect(ui.sliderB,SIGNAL(valueChanged(int)),this,SLOT(colChanged()));
	connect(ui.sliderR,SIGNAL(valueChanged(int)),this,SLOT(colChanged()));
	connect(ui.sliderG,SIGNAL(valueChanged(int)),this,SLOT(colChanged()));
	connect(ui.layPal,SIGNAL(valueChanged(int)),this,SLOT(palChange()));
	connect(ui.tilePal,SIGNAL(valueChanged(int)),this,SLOT(palChange()));
	connect(ui.tileNum,SIGNAL(valueChanged(int)),this,SLOT(tilChange(int)));
	connect(ui.tiledit,SIGNAL(tileChanged(int)),this,SLOT(tilChange(int)));
	connect(ui.tbpicktile,SIGNAL(released()),this,SLOT(pickTile()));
	connect(&bigview,SIGNAL(colChanged(int)),ui.tileNum,SLOT(setValue(int)));

	connect(ui.actOpenPrj,SIGNAL(triggered()),this,SLOT(openPrj()));
	connect(ui.actSavePrj,SIGNAL(triggered()),this,SLOT(savePrj()));

	connect(ui.actTMEdit,SIGNAL(triggered()),tedit,SLOT(show()));
}

TMEdit::TMEdit(QWidget *p):QDialog(p) {
	teui.setupUi(this);

	teui.tileLine->type = ML_TILEROW;
	teui.tilemap->type = ML_TILEMAP;
	teui.tileShow->type = ML_TILESHOW;

	teui.tilemap->xpos = 0;
	teui.tilemap->ypos = 0;

	connect(teui.tbLeft,SIGNAL(released()),this,SLOT(tileRowUp()));
	connect(teui.tbRight,SIGNAL(released()),this,SLOT(tileRowDn()));
	connect(teui.l2box,SIGNAL(stateChanged(int)),teui.tilemap,SLOT(update()));

	connect(teui.tbSave,SIGNAL(released()),this,SLOT(saveTMap()));
	connect(teui.tbOpen,SIGNAL(released()),this,SLOT(openTMap()));
}

void TMEdit::tileRowUp() {
	if (teui.tileLine->row == 0) return;
	teui.tileLine->row--;
	teui.tileLine->update();
}

void TMEdit::tileRowDn() {
	if (teui.tileLine->row > 55) return;
	teui.tileLine->row++;
	teui.tileLine->update();
}

void TMEdit::openTMap() {
	QString path = QFileDialog::getOpenFileName(this,"Open tilemap",tmPath,"tilemap (*.tsm);");
	if (path == "") return;
	QFile file(path);
	if (file.open(QFile::ReadOnly)) {
		tmPath = path;
		int x,y,idx,tile;
		unsigned char ch;
		idx = 0;
		for (y = 0; y < 64; y++) {
			for (x = 0; x < 64; x++) {
				file.getChar((char*)&ch);
				tile = ch;
				file.getChar((char*)&ch);
				tile |= ((ch & 0x0f) << 8);
				tileMap[0][idx + x] = tile;
			}
			for (x = 0; x < 64; x++) {
				file.getChar((char*)&ch);
				tile = ch;
				file.getChar((char*)&ch);
				tile |= ((ch & 0x0f) << 8);
				tileMap[1][idx + x] = tile;
			}
			idx += 64;
		}
	}
}

void TMEdit::saveTMap() {
	QString path = QFileDialog::getSaveFileName(this,"Save tilemap",tmPath,"tilemap (*.tsm);");
	if (path == "") return;
	QFile file(path);
	if (file.open(QFile::WriteOnly)) {
		tmPath = path;
		int x,y,idx,tile;
		idx = 0;
		for (y = 0; y < 64; y++) {
			for (x = 0; x < 64; x++) {
				tile = tileMap[0][idx + x] & 0xfff;
				tile |= (tiles[tile].pal & 3 << 12);
				file.putChar(tile & 0xff);
				file.putChar((tile >> 8) & 0xff);
			}
			for (x = 0; x < 64; x++) {
				tile = tileMap[1][idx + x] & 0xfff;
				tile |= (tiles[tile].pal & 3 << 12);
				file.putChar(tile & 0xff);
				file.putChar((tile >> 8) & 0xff);
			}
			idx += 64;
		}
	}
}

void TMEdit::keyPressEvent(QKeyEvent* ev) {
	switch (ev->key()) {
		case Qt::Key_W:
			teui.tilemap->ypos--;
			if (teui.tilemap->ypos < 0) teui.tilemap->ypos = 0;
			teui.tilemap->update();
			break;
		case Qt::Key_S:
			teui.tilemap->ypos++;
			if (teui.tilemap->ypos > 32) teui.tilemap->ypos = 32;
			teui.tilemap->update();
			break;
		case Qt::Key_A:
			teui.tilemap->xpos--;
			if (teui.tilemap->xpos < 0) teui.tilemap->xpos = 0;
			teui.tilemap->update();
			break;
		case Qt::Key_D:
			teui.tilemap->xpos++;
			if (teui.tilemap->xpos > 32) teui.tilemap->xpos = 32;
			teui.tilemap->update();
			break;
	}
}

void MWin::changeCol(int idx) {
	if (idx & 0x0f) {
		ui.colwid->setEnabled(true);
		flag |= BLK_CHA;
		ui.sliderB->setValue(pal[idx].b);
		ui.sliderR->setValue(pal[idx].r);
		flag &= ~BLK_CHA;
		ui.sliderG->setValue(pal[idx].g);
	} else {
		ui.colwid->setEnabled(false);
	}
}

void MWin::colChanged() {
	if (flag & BLK_CHA) return;
	int idx = ui.paledit->colidx;
	pal[idx].b = ui.sliderB->value();
	pal[idx].r = ui.sliderR->value();
	pal[idx].g = ui.sliderG->value();
	ui.spinB->setValue(tslCoLevs[pal[idx].b]);
	ui.spinR->setValue(tslCoLevs[pal[idx].r]);
	ui.spinG->setValue(tslCoLevs[pal[idx].g]);
	pal[idx].col.setBlue(tslCoLevs[pal[idx].b]);
	pal[idx].col.setRed(tslCoLevs[pal[idx].r]);
	pal[idx].col.setGreen(tslCoLevs[pal[idx].g]);
	ui.paledit->update();
	ui.tilePalGrid->update();
	ui.tiledit->update();
}

void MWin::palChange() {
	tiles[ui.tileNum->value()].pal = ui.tilePal->value();
	ui.tilePalGrid->update();
	ui.tiledit->update();
}

void MWin::tilChange(int idx) {
	idx &= 0xfff;
	ui.tileNum->setValue(idx);
	ui.tilePal->setValue(tiles[idx].pal);
	ui.tiledit->update();
}

void MWin::pickTile() {
	bigview.show();
}

void drawTileMap(MLabel* lab) {
	QImage bigimg(512,512,QImage::Format_RGB888);
	int idx,x,y;
	int xpos,ypos;
	int col;
	for (idx = 0; idx < 4096; idx++) {
		xpos = (idx & 0x3f) << 3;
		ypos = (idx & 0xfc0) >> 3;
		for (y = 0; y < 8; y++) {
			for (x = 0; x < 8; x++) {
				col = (ui.layPal->value() << 6) | (tiles[idx].pal << 4) | tiles[idx].data[x + y * 8];
				bigimg.setPixel(xpos + x, ypos + y, qRgb(pal[col].col.red(), pal[col].col.green(),pal[col].col.blue()));
			}
		}
	}
	QPixmap pxm = QPixmap::fromImage(bigimg);
	lab->setPixmap(pxm);
}

// #######################################

MLabel::MLabel(QWidget *p):QLabel(p) {
	colidx = 0;
	row = 0;
}

#define	TILE_DBLSZ	1
#define	TILE_UNDER	(1<<1)
#define	TILE_TRANS	(1<<2)
#define	TILE_NZ		(1<<3)

void MLabel::drawTile(int xpos, int ypos, int idx, int flag, QPainter* pnt) {
	if ((idx == 0) && (flag & TILE_NZ)) return;
	int x,y,col;
	QColor clr;
	for (y = 0; y < 8; y++) {
		for (x = 0; x < 8; x++) {
			col = (ui.layPal->value() << 6) | (tiles[idx].pal << 4) | tiles[idx].data[x + (y << 3)];
			if ((col & 0x0f) || (~flag & TILE_TRANS)) {
				clr = pal[col].col;
				if (flag & TILE_UNDER) {
					clr.setRgb(clr.red() >> 2, clr.green() >> 2, clr.blue() >> 2);
				}
				pnt->setPen(clr);
				if (flag & TILE_DBLSZ) {
					pnt->drawPoint(xpos + x * 2, ypos + y * 2);
					pnt->drawPoint(xpos + x * 2, ypos + y * 2 + 1);
					pnt->drawPoint(xpos + x * 2 + 1, ypos + y * 2);
					pnt->drawPoint(xpos + x * 2 + 1, ypos + y * 2 + 1);
				} else {
					pnt->drawPoint(xpos + x, ypos + y);
				}
			}
		}
	}
}

void MLabel::drawEditBox(int xst, int yst, int num, QColor bcol, QPainter* pnt) {
	Tile* til = &tiles[num & 0xfff];
	int tpal = ((ui.layPal->value() << 2) | til->pal) << 4;
	int x,y;
	int idx = 0;
	pnt->setPen(bcol);
	for (y = yst; y < (yst + 128); y += 16) {
		for (x = xst; x < (xst + 128); x += 16) {
			pnt->setBrush(pal[tpal | (til->data[idx] & 0x0f)].col);
			pnt->drawRect(x,y,16,16);
			idx++;
		}
	}
}

void MLabel::paintEvent(QPaintEvent*) {
	int idx,tnum; //,tpal;
	int x,y;
//	int xpos,ypos;
//	Tile* til = &tiles[ui.tileNum->value()];
	QPainter pnt;
	pnt.begin(this);
	switch(type) {
		case ML_PALEDIT:
			idx = 0;
			for (y = 0; y < 256; y += 16) {
				for (x = 0; x < 256; x += 16) {
					pnt.setPen((idx == colidx) ? Qt::red : Qt::gray);
					pnt.setBrush(pal[idx].col);
					pnt.drawRect(x,y,16,16);
					idx++;
				}
			}
			break;
		case ML_TILEPAL:
			idx = ((ui.layPal->value() << 2) | ui.tilePal->value()) << 4;
			for (x = 0; x < 256; x += 16) {
				pnt.setPen((idx == colidx) ? Qt::red : Qt::gray);
				pnt.setBrush(pal[idx].col);
				pnt.drawRect(x,0,16,16);
				idx++;
			}
			break;
		case ML_TILEDIT:
			tnum = ui.tileNum->value();

			pnt.fillRect(0,0,256,256,Qt::black);

			drawEditBox(-66,-66,tnum - 65,Qt::darkGray,&pnt);
			drawEditBox(64,-66,tnum - 64,Qt::darkGray,&pnt);
			drawEditBox(194,-66,tnum - 63,Qt::darkGray,&pnt);

			drawEditBox(-66,64,tnum - 1,Qt::darkGray,&pnt);
			drawEditBox(64,64,tnum,Qt::black,&pnt);
			drawEditBox(194,64,tnum + 1,Qt::darkGray,&pnt);

			drawEditBox(-66,194,tnum + 63,Qt::darkGray,&pnt);
			drawEditBox(64,194,tnum + 64,Qt::darkGray,&pnt);
			drawEditBox(194,194,tnum + 65,Qt::darkGray,&pnt);
			break;
		case ML_BIGVIEW:
			for (idx = 0; idx < 4096; idx++) {
				xpos = (idx & 0x3f) << 3;
				ypos = (idx & 0xfc0) >> 3;
				drawTile(xpos,ypos,idx,0,&pnt);
			}
			break;
		case ML_TILEROW:
			for (y = 0; y < 8; y++) {
				for (x = 0; x < 64; x++) {
					drawTile(x << 3, y << 3, ((row + y) << 6) + x,0,&pnt);
				}
			}
			break;
		case ML_TILESHOW:
			drawTile(0,0,teui.tileLine->colidx,TILE_DBLSZ,&pnt);
			break;
		case ML_TILEMAP:
			int idx = (ypos << 6) + xpos;
			for (y = 0; y < 32; y++) {
				for (x = 0; x < 32; x++) {
					if (teui.l2box->isChecked()) {
						drawTile(x << 4, y << 4, tileMap[0][idx + (y << 6) + x],TILE_DBLSZ | TILE_UNDER,&pnt);
						drawTile(x << 4, y << 4, tileMap[1][idx + (y << 6) + x],TILE_DBLSZ | TILE_TRANS | TILE_NZ,&pnt);
					} else {
						drawTile(x << 4, y << 4, tileMap[0][idx + (y << 6) + x],TILE_DBLSZ,&pnt);
					}
				}
			}
			break;
	}
	pnt.end();
}

void MLabel::mousePressEvent(QMouseEvent* ev) {
	int ctil = ui.tileNum->value();
	switch (type) {
		case ML_PALEDIT:
			colidx = (ev->y() & 0xf0) | (ev->x() >> 4);
			update();
			emit colChanged(colidx);
			break;
		case ML_TILEPAL:
			colidx = (((ui.layPal->value() << 2) | ui.tilePal->value()) << 4) | (ev->x() >> 4);
			update();
			break;
		case ML_TILEDIT:
			if (ev->y() < 64) {
				if (ev->x() < 64) emit tileChanged(ctil - 65);
				else if (ev->x() > 192) emit tileChanged(ctil - 63);
				else emit tileChanged(ctil - 64);
			} else if (ev->y() > 192) {
				if (ev->x() < 64) emit tileChanged(ctil + 63);
				else if (ev->x() > 192) emit tileChanged(ctil + 65);
				else emit tileChanged(ctil + 64);
			} else {
				if (ev->x() < 64) emit tileChanged(ctil - 1);
				else if (ev->x() > 192) emit tileChanged(ctil + 1);
				else mouseMoveEvent(ev);
			}
			break;
		case ML_BIGVIEW:
			colidx = ((ev->y() & 0x1f8) << 3) | ((ev->x() & 0x1f8) >> 3);
			emit colChanged(colidx);
			hide();
			break;
		case ML_TILEROW:
			colidx = ((row + (ev->y() >> 3)) << 6) | (ev->x() >> 3);
			teui.tileShow->update();
			break;
		case ML_TILEMAP:
			mouseMoveEvent(ev);
			break;
	}
}

void MLabel::mouseMoveEvent(QMouseEvent *ev) {
	switch(type) {
		case ML_TILEDIT:
			if (ev->x() < 64) break;
			if (ev->y() < 64) break;
			if (ev->x() > 191) break;
			if (ev->y() > 191) break;
			colidx = (((ev->y() - 64) & 0xf0) >> 1) | ((ev->x() - 64) >> 4);
			if (ev->buttons() & Qt::LeftButton)
				tiles[ui.tileNum->value()].data[colidx] = ui.tilePalGrid->colidx & 0x0f;
			if (ev->buttons() & Qt::RightButton)
				tiles[ui.tileNum->value()].data[colidx] = 0x00;
			update();
			break;
		case ML_TILEMAP:
			if (ev->x() < 0) break;
			if (ev->x() > 511) break;
			if (ev->y() < 0) break;
			if (ev->y() > 511) break;
			colidx = (ypos + (ev->y() >> 4)) << 6;
			colidx += (xpos + (ev->x() >> 4));
			row = (teui.l2box->isChecked()) ? 1 : 0;
			if (ev->buttons() & Qt::LeftButton)
				tileMap[row][colidx] = teui.tileLine->colidx;
			if (ev->buttons() & Qt::RightButton) {
				teui.tileLine->colidx = tileMap[row][colidx];
				teui.tileShow->update();
			}
			update();
			break;
	}
}

// open-save

void MWin::savePrj() {
	QString path = QFileDialog::getSaveFileName(this,"Save all","","TSConf (*.tst)");
	if (path == "") return;

	saveTiles(path);
	path.remove(path.size() - 3, 3);
	path.append("tsp");
	savePal(path);
}

void MWin::saveTiles(QString path) {
	QFile file(path);
	if (!file.open(QFile::WriteOnly)) return;

	unsigned char tdata[256 * 512];
	int idx,sdx;
	int adr;
	unsigned char col;
	for (idx = 0; idx < 4096; idx++) {
		adr = ((idx & 0xfc0) << 5) | ((idx & 0x3f) << 2);
		for (sdx = 0; sdx < 64; sdx += 2) {
			col = (tiles[idx].data[sdx] << 4) | (tiles[idx].data[sdx + 1]);
			tdata[adr + ((sdx & 7) >> 1) + ((sdx & 0x38) << 5)] = col;
		}
	}
	file.write((char*)tdata,256 * 512);
	file.close();

	path.remove(path.size() - 3, 3);
	path.append("idx");
	file.setFileName(path);
	if (file.open(QFile::WriteOnly)) {
		file.putChar((ui.tilePal->value() & 3) << 6);
		for (idx = 0; idx < 4096; idx++) {
			file.putChar((tiles[idx].pal & 3) << 4);
		}
		file.close();
	}
}

void MWin::savePal(QString path) {
	QFile file(path);
	if (!file.open(QFile::WriteOnly)) return;
	int idx;
	int col;
	for (idx = 0; idx < 256; idx++) {
		if (idx & 15) {
			col = (pal[idx].r << 10) | (pal[idx].g << 5) | pal[idx].b;
		} else {
			col = 0;
		}
		file.putChar((col & 0xff00) >> 8);
		file.putChar(col & 0xff);
	}
	file.close();
}

void MWin::openPrj() {
	QString path = QFileDialog::getOpenFileName(this,"Open project","","TSConf (*.tst)");
	if (path == "") return;
	openTiles(path);
	path.remove(path.size() - 3, 3);
	path.append("tsp");
	openPal(path);

}

void MWin::openPal(QString path) {
	QFile file(path);
	if (!file.open(QFile::ReadOnly)) return;
	int col;
	unsigned char hcol;
	int idx;
	for (idx = 0; idx < 256; idx++) {
		file.getChar((char*)&hcol);
		col = hcol << 8;
		file.getChar((char*)&hcol);
		col |= hcol;
		if (idx & 15) {
			pal[idx].r = (col >> 10) & 0x1f;
			pal[idx].g = (col >> 5) & 0x1f;
			pal[idx].b = col & 0x1f;
			pal[idx].col.setBlue(tslCoLevs[pal[idx].b]);
			pal[idx].col.setRed(tslCoLevs[pal[idx].r]);
			pal[idx].col.setGreen(tslCoLevs[pal[idx].g]);
		}
	}
	file.close();
	ui.paledit->update();
	ui.tilePalGrid->update();
	ui.tiledit->update();
}

void MWin::openTiles(QString path) {
	QFile file(path);
	if (!file.open(QFile::ReadOnly)) return;
	char col;
	int x,y,line,px;
	unsigned char* ptr;
	for (y = 0; y < 64; y++) {
		for (line = 0; line < 8; line++) {
			for(x = 0; x < 64; x++) {
				ptr = &(tiles[(y << 6) | x].data[line << 3]);
				for (px = 0; px < 4; px++) {
					file.getChar(&col);
					*(ptr++) = (col & 0xf0) >> 4;
					*(ptr++) = col & 0x0f;
				}
			}
		}
	}

	file.close();

	path.remove(path.size() - 3, 3);
	path.append("idx");
	file.setFileName(path);
	char tmp;
	if (file.open(QFile::ReadOnly)) {
		file.getChar(&tmp);
		ui.tilePal->setValue((tmp >> 6) & 3);
		for (int idx = 0; idx < 4096; idx++) {
			file.getChar(&tmp);
			tiles[idx].pal = (tmp >> 4) & 3;
		}
		file.close();
	}

	ui.tiledit->update();
}
