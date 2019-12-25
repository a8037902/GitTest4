#pragma once

#include <QDialog>
#include <qpainter.h>
#include <qmath.h>
#include "ThumbWidget.h"

namespace Ui { class TabIconDialog; };

typedef BOOL(WINAPI*pfBitBlt)(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);

class TabIconDialog : public QDialog
{
	Q_OBJECT

public:
	TabIconDialog(pfBitBlt pfBitBlt,QWidget *parent = Q_NULLPTR);
	~TabIconDialog();

	void setWindowsList(QList<QWidget*> list, int index);
	QList<ThumbWidget*> getList();
	void showToGridLayout();
	void toNext();
	int getIndex() {
		return mBtnList.size() - 1 - index;
	}
	void clearThumbList();
	void removeThumbWidget(ThumbWidget* pThumb);
	ThumbWidget* getThumbWidget(HWND hWnd);
	pfBitBlt m_pfBitBlt;

protected:
	virtual void paintEvent(QPaintEvent *event);

private:
	Ui::TabIconDialog *ui;
	int index;
	QList<ThumbWidget*> mBtnList;
};
