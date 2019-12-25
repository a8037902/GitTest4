#pragma once

#include "IDrawable.h"

class QRectangle : public IDrawable
{
	Q_OBJECT

public:
	QRectangle(QPoint startPoint, DrawableProperty pro, QObject *parent = 0);
	~QRectangle();

	virtual void Draw(QPainter& painter);

	void setEndPoint(QPoint endPoint);
private:
	QPoint m_startPoint;
	QPoint m_endPoint;
};
