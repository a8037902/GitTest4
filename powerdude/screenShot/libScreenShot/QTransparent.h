#pragma once

#include <QObject>
#include "IDrawable.h"
#include <QList>

class QTransparent : public IDrawable
{
	Q_OBJECT

public:
	QTransparent(QPoint startPoint, DrawableProperty pro, QObject *parent = 0);
	~QTransparent();
	virtual void Draw(QPainter& painter);
	void addPoint(QPoint& point);
protected:
	QPoint m_startPoint;
	QList<QPoint> m_points;
};
