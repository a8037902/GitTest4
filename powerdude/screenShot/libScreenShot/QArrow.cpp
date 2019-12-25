#include "QArrow.h"

QArrow::QArrow(QPoint startPoint, DrawableProperty pro, QObject *parent)
	: IDrawable(DrawableType::ArrowObj,pro, parent),
	m_startPoint(startPoint)
{

}

QArrow::~QArrow()
{

}

void QArrow::Draw(QPainter& painter)
{
	if (m_bCanDrawed) {
		painter.save();
		double par = 15.0;
		if (m_property.size.PenSizeEnum == PenSizeEnum::Small) {
			par = 10.0;
		}else if (m_property.size.PenSizeEnum == PenSizeEnum::Middle) {
			par = 15.0;
		}
		else if (m_property.size.PenSizeEnum == PenSizeEnum::Big) {
			par = 18.0;
		}
		double slopy = atan2((m_endPoint.y() - m_startPoint.y()), (m_endPoint.x() - m_startPoint.x()));
		double cosy = cos(slopy);
		double siny = sin(slopy);
		QPoint point1 = QPoint(m_endPoint.x() + int(-par * cosy - (par / 2.0*siny)), m_endPoint.y() + int(-par * siny + (par / 2.0*cosy)));
		QPoint point2 = QPoint(m_endPoint.x() + int(-par * cosy + (par / 2.0*siny)), m_endPoint.y() - int(par / 2.0*cosy + par * siny));

		int offsetx = int(par*siny / 3);
		int offsety = int(par*cosy / 3);
		QPoint point3, point4;
		point3 = QPoint(m_endPoint.x() + int(-par * cosy - (par / 2.0*siny)) + offsetx, m_endPoint.y() + int(-par * siny + (par / 2.0*cosy)) - offsety);
		point4 = QPoint(m_endPoint.x() + int(-par * cosy + (par / 2.0*siny) - offsetx), m_endPoint.y() - int(par / 2.0*cosy + par * siny) + offsety);

		QPen pen;
		pen.setColor(QColor(m_property.color));
		
		pen.setWidth(1);
		painter.setPen(pen);
		painter.setRenderHint(QPainter::Antialiasing);

		QPainterPath path;
		path.moveTo(point1);
		path.lineTo(m_endPoint);
		path.lineTo(point2);
		path.lineTo(point4);
		path.lineTo(m_startPoint);
		path.lineTo(point3);
		path.lineTo(point1);
		painter.drawPath(path);
		painter.fillPath(path, QColor(m_property.color));
		painter.restore();
	}
}

void QArrow::setEndPoint(QPoint endPoint)
{
	m_endPoint = endPoint;
	if (abs(m_endPoint.x() - m_startPoint.x()) > 3 || abs(m_endPoint.y() - m_startPoint.y()) > 3) {
		m_bCanDrawed = true;
	}
}