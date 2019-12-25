#pragma once

#include <QObject>
#include "IDrawable.h"
#include <QImage>

class QMask : public IDrawable
{
	Q_OBJECT

public:
	QMask(QPoint startPoint, DrawableProperty pro, QImage* image, QObject *parent = 0);
	~QMask();
	void setEndPoint(QPoint endPoint);
	virtual void Draw(QPainter& painter);
	void setDrawBorder(bool bDrawBorder);
private:
	QRgb getAveragePixel(int xStart,int yStart);
	void setAveragePixel(QRgb color,int xStart,int yStart, QImage* pImage);
protected:
	QPoint m_startPoint;
	QPoint m_endPoint;
	QImage* m_screenImage;
	int		m_iRecSize;
	bool   m_bDrawBorder;
};
