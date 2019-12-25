#pragma once

#include <QObject>
#include "IDrawable.h"
#include <QList>

class QBrushDrawer : public IDrawable
{
	Q_OBJECT

public:
	QBrushDrawer(QPoint startPoint, DrawableProperty pro,QObject *parent = 0);
	~QBrushDrawer();

	virtual void Draw(QPainter& painter);
	void addPoint(QPoint& point);

protected:
	QPoint m_startPoint;
	QList<QPoint> m_points;
};
