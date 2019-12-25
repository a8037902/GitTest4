#include "TextMovingWidget.h"
#include <QTimer>
#include <QPainter>
#include <QPropertyAnimation>

TextMovingWidget::TextMovingWidget(QWidget *parent)
	: QWidget(parent), m_direction(1), m_x(0), state(0), cnt(0),index(0)
{
	//QPalette pe;
	//pe.setColor(QPalette::Background, /*QColor(0, 0, 0, 100)*/Qt::black);
	//this->setPalette(pe);

	_timer = new QTimer(this);
	connect(_timer, SIGNAL(timeout()), this, SLOT(slot_UpdateTextGeometry()));
	
}

TextMovingWidget::~TextMovingWidget() {}

void TextMovingWidget::addText(const QString &text)
{
	textList.push_back(text);
	if(!_timer->isActive())_timer->start(20);
}

void TextMovingWidget::paintEvent(QPaintEvent *e)
{
	QWidget::paintEvent(e);
	QPainter painter(this);

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

	painter.fillPath(path, QColor(0, 0, 0,100));

	painter.setPen(Qt::white);
	//QFontMetrics metric(font());
	//int y = (height() - metric.height()) / 2;
	//painter.drawText(m_x, y, metric.width(_text), metric.height(), Qt::AlignLeft, _text);
	if (textList.size() > 0) {
		QFont font = painter.font();
		font.setPixelSize(12);
		painter.setFont(font);
		if (state == 0) {
			painter.drawText(m_x, 0, this->width(), this->height(), Qt::AlignVCenter | Qt::AlignHCenter, textList[index]);
		}
		else if (state == 1) {
			painter.drawText(m_x, 0, this->width(), this->height(), Qt::AlignVCenter | Qt::AlignHCenter, textList[index]);
			painter.drawText(m_x + this->width(), 0, this->width(), this->height(), Qt::AlignVCenter | Qt::AlignHCenter, textList[(index + 1) % textList.size()]);
		}
	}
}

inline int TextMovingWidget::getDirection(const QString &text)
{
	QFontMetrics metrics(font());
	int tw = metrics.width(text);
	if (tw <= width())
	{
		if (m_x + tw > width())
			return 0;
		else if (m_x <= 0)
			return 1;
		return m_direction;
	}
	else
	{
		if (m_x == 10)
			return 0;
		else if (m_x <= width() - tw - 10)
			return 1;
		return m_direction;
	}
}

void TextMovingWidget::slot_UpdateTextGeometry()
{
	//QFontMetrics metric(font());
	//int tw = metric.width(_text);
	//if (tw > width()) //If the text width greater than widget width
	//{
	//	if (m_direction) //right
	//		m_x++;
	//	else          //left
	//		m_x--;
	//}
	//else
	//{
	//	if (m_direction) //right
	//		m_x++;
	//	else          //left
	//		m_x--;
	//}
	//m_direction = getDirection(_text);

	if (state == 0) {
		if (++cnt >= 150) {
			state = 1;
			cnt = 0;
		}
	}
	else if (state == 1) {
		m_x-=15;
		if (m_x + width() <= 0) {
			state = 0;
			m_x = 0;
			index = (index + 1) % textList.size();
		}
	}
	update();

}
