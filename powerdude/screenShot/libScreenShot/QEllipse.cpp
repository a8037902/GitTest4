#include "QEllipse.h"

QEllipse::QEllipse(QPoint startPoint, DrawableProperty pro, QObject *parent)
	:IDrawable(DrawableType::EllipseObj,pro, parent),
	m_startPoint(startPoint)
{

}

QEllipse::~QEllipse()
{

}


void QEllipse::Draw(QPainter& painter)
{
	if (m_bCanDrawed) {
		painter.save();
		QPen pen;
		if (m_property.size.PenSizeEnum == PenSizeEnum::Small) {
			pen.setWidth(1);
		}
		else if (m_property.size.PenSizeEnum == PenSizeEnum::Middle) {
			pen.setWidth(3);
		}
		else if (m_property.size.PenSizeEnum == PenSizeEnum::Big) {
			pen.setWidth(5);
		}
		else {
			pen.setWidth(1);
		}
		pen.setColor(QColor(m_property.color));
		painter.setPen(pen);
		painter.setRenderHint(QPainter::Antialiasing);
		int x2 = m_startPoint.x() < m_endPoint.x() ? m_startPoint.x() : m_endPoint.x();
		int y2 = m_startPoint.y() < m_endPoint.y() ? m_startPoint.y() : m_endPoint.y();
		painter.drawEllipse(x2, y2, abs(m_endPoint.x() - m_startPoint.x()), abs(m_endPoint.y() - m_startPoint.y()));
		painter.restore();
	}
}

void QEllipse::setEndPoint(QPoint endPoint)
{
	m_endPoint = endPoint;
	if (abs(m_endPoint.x() - m_startPoint.x()) > 3 || abs(m_endPoint.y() - m_startPoint.y()) > 3) {
		m_bCanDrawed = true;
	}
}