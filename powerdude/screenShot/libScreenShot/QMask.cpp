#include "QMask.h"

QMask::QMask(QPoint startPoint, DrawableProperty pro, QImage* image, QObject *parent)
	: IDrawable(DrawableType::MaskObj,pro, parent),
	m_startPoint(startPoint),
	m_screenImage(image),
	m_iRecSize(4),
	m_bDrawBorder(true)
{
	if (MaskSize::Mask_6 == m_property.size.MaskSize) {
		m_iRecSize = 4;
	}else if (MaskSize::Mask_8 == m_property.size.MaskSize) {
		m_iRecSize = 6;
	}else if (MaskSize::Mask_10 == m_property.size.MaskSize) {
		m_iRecSize = 8;
	}else if (MaskSize::Mask_12 == m_property.size.MaskSize) {
		m_iRecSize = 10;
	}else if (MaskSize::Mask_14 == m_property.size.MaskSize) {
		m_iRecSize = 12;
	}else if (MaskSize::Mask_16 == m_property.size.MaskSize) {
		m_iRecSize = 14;
	}
}

QMask::~QMask()
{

}

void QMask::setEndPoint(QPoint endPoint)
{
	m_endPoint = endPoint;
	if (abs(m_endPoint.x() - m_startPoint.x()) > 3 || abs(m_endPoint.y() - m_startPoint.y()) > 3) {
		m_bCanDrawed = true;
	}
}

void QMask::Draw(QPainter& painter)
{
	if (m_bCanDrawed) {
		painter.save();
		QImage image = QImage(abs(m_endPoint.x() - m_startPoint.x()), abs(m_endPoint.y() - m_startPoint.y()), QImage::Format_ARGB32);

		for (int i = qMin(m_startPoint.x(), m_endPoint.x()); i < qMax(m_startPoint.x(),m_endPoint.x()); i += m_iRecSize) {
			for (int j = qMin(m_startPoint.y(), m_endPoint.y()); j < qMax(m_startPoint.y(),m_endPoint.y()); j += m_iRecSize) {
				setAveragePixel(getAveragePixel(i, j), i - qMin(m_startPoint.x(), m_endPoint.x()), j - qMin(m_startPoint.y(), m_endPoint.y()), &image);
			}
		}
		painter.drawImage(QPoint(qMin(m_startPoint.x(),m_endPoint.x()), qMin(m_startPoint.y(),m_endPoint.y())), image);
		if (m_bDrawBorder) {
			QPen pen(Qt::PenStyle::DotLine);
			pen.setWidth(1);
			pen.setColor(Qt::gray);
			painter.setPen(pen);
			painter.setRenderHint(QPainter::Antialiasing);
			int x2 = m_startPoint.x() < m_endPoint.x() ? m_startPoint.x() : m_endPoint.x();
			int y2 = m_startPoint.y() < m_endPoint.y() ? m_startPoint.y() : m_endPoint.y();
			painter.drawRect(x2, y2, abs(m_endPoint.x() - m_startPoint.x()), abs(m_endPoint.y() - m_startPoint.y()));
		}
		painter.restore();
	}
}

void QMask::setDrawBorder(bool bDrawBorder)
{
	m_bDrawBorder = bDrawBorder;
}

void QMask::setAveragePixel(QRgb color, int xStart, int yStart, QImage* pImage)
{
	for (int i = xStart; i < xStart + m_iRecSize; i++) {
		for (int j = yStart; j < yStart + m_iRecSize; j++) {
			if (i < pImage->width() && j < pImage->height()) {
				pImage->setPixel(i, j, color);
			}
		}
	}
}

QRgb QMask::getAveragePixel(int xStart,int yStart)
{
	int r = 0, g = 0, b = 0, a = 0,count = 0;
	for (int i = xStart; i < xStart + m_iRecSize; i++) {
		for (int j = yStart; j < yStart + m_iRecSize; j++) {
			if (i < m_screenImage->width() && j < m_screenImage->height()) {
				QRgb pixel = m_screenImage->pixel(i, j);
				r += qRed(pixel);
				g += qGreen(pixel);
				b += qBlue(pixel);
				a += qAlpha(pixel);
				count++;
			}
		}
	}
	if (count <= 0) {
		return qRgba(0, 0, 0, 0);
	}
	return qRgba(r / count, g / count, b / count, a / count);
}
