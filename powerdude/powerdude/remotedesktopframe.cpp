#include "remotedesktopframe.h"
#include <QPainter>


RemoteDesktopFrame::RemoteDesktopFrame(QWidget* parent)
	:AutoHideFrame(parent)
{
	/*QPalette pe;
	pe.setColor(QPalette::Background, Qt::white);
	this->setPalette(pe);
	this->setAutoFillBackground(true);*/
}


RemoteDesktopFrame::~RemoteDesktopFrame()
{
}

void RemoteDesktopFrame::paintEvent(QPaintEvent *e)
{

	/*QPainter painter(this);

	const qreal radius = 5;
	QRectF rect = QRect(0, 0, this->width(), this->height());

	QPainterPath path;

	path.moveTo(rect.bottomRight() - QPointF(0, radius));
	path.lineTo(rect.topRight() + QPointF(0, radius));
	path.arcTo(QRectF(QPointF(rect.topRight() - QPointF(radius * 2, 0)), QSize(radius * 2, radius * 2)), 0, 90);
	path.lineTo(rect.topLeft() + QPointF(radius, 0));
	path.arcTo(QRectF(QPointF(rect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
	path.lineTo(rect.bottomLeft() - QPointF(0, radius));
	path.arcTo(QRectF(QPointF(rect.bottomLeft() - QPointF(0, radius * 2)), QSize(radius * 2, radius * 2)), 180, 90);
	path.lineTo(rect.bottomLeft() + QPointF(radius, 0));
	path.arcTo(QRectF(QPointF(rect.bottomRight() - QPointF(radius * 2, radius * 2)), QSize(radius * 2, radius * 2)), 270, 90);

	painter.fillPath(path, QColor(66,78,87));*/

	QWidget::paintEvent(e);
}
