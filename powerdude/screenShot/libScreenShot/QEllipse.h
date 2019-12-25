#pragma once

#include "IDrawable.h"

class QEllipse : public IDrawable
{
	Q_OBJECT

public:
	QEllipse(QPoint startPoint, DrawableProperty pro, QObject *parent = 0);
	~QEllipse();

	virtual void Draw(QPainter& painter);
	void setEndPoint(QPoint endPoint);
private:
	QPoint m_startPoint;
	QPoint m_endPoint;
};
