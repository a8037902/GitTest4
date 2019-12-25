#include "translucentbutton.h"
#include <QPainter>


TranslucentButton::TranslucentButton(QWidget *parent)
	: QPushButton(parent)
{
	this->setFlat(true);

	QPalette pe;
	pe.setColor(QPalette::ButtonText, Qt::white);
	this->setPalette(pe);
	this->setStyleSheet(QString("QPushButton:pressed{background-color:rgb(0, 0, 0,0)}"));
}


TranslucentButton::~TranslucentButton()
{
}

void TranslucentButton::paintEvent(QPaintEvent *event)
{
	Q_UNUSED(event);

	QPainter painter(this);
	const qreal radius = 10;
	QRectF rect = QRect(0, 0, this->width()-1, this->height()-1);

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

	painter.fillPath(path, QColor(0, 0, 0,125));

	painter.setPen(Qt::gray);
	painter.drawPath(path);
	//painter.drawEllipse(2,2,this->width()-4,this->height()-4);

	QPushButton::paintEvent(event);
}
