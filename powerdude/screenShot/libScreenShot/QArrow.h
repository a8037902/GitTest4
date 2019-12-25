#pragma once

#include "IDrawable.h"

class QArrow : public IDrawable
{
	Q_OBJECT

public:
	QArrow(QPoint startPoint, DrawableProperty pro, QObject *parent = 0);
	~QArrow();
	virtual void Draw(QPainter& painter);
	void setEndPoint(QPoint endPoint);
private:
	QPoint m_startPoint;
	QPoint m_endPoint;
};
