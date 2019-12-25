#include "QBrush.h"

QBrushDrawer::QBrushDrawer(QPoint startPoint, DrawableProperty pro, QObject *parent)
	: IDrawable(DrawableType::BrushObj,pro, parent),
	m_startPoint(startPoint)
{

}

QBrushDrawer::~QBrushDrawer()
{

}

void QBrushDrawer::addPoint(QPoint& point)
{
	m_points.push_back(point);
	m_bCanDrawed = true;
}

void QBrushDrawer::Draw(QPainter& painter)
{
	if (m_bCanDrawed) {
		painter.save();
		QPen pen;
		pen.setColor(QColor(qRed(m_property.color), qGreen(m_property.color), qBlue(m_property.color), 0xff));
		if (m_property.size.PenSizeEnum == PenSizeEnum::Small) {
			pen.setWidth(3);
		}
		else if (m_property.size.PenSizeEnum == PenSizeEnum::Middle) {
			pen.setWidth(5);
		}
		else if (m_property.size.PenSizeEnum == PenSizeEnum::Big) {
			pen.setWidth(8);
		}
		else {
			pen.setWidth(3);
		}
		painter.setPen(pen);
		painter.setRenderHint(QPainter::Antialiasing);
		QPainterPath path;
		path.moveTo(m_startPoint);
		for (QList<QPoint>::iterator i = m_points.begin(); i != m_points.end(); i++) {
			path.lineTo(*i);
		}
		painter.drawPath(path);
		painter.restore();
	}
}