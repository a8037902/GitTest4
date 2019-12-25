#include "QText.h"

QTextDrawer::QTextDrawer(QRect posRect, DrawableProperty pro, QString strText, QObject *parent)
	: IDrawable(DrawableType::TextObj,pro, parent),
	m_posRect(posRect),
	m_strText(strText){

}

QTextDrawer::~QTextDrawer()
{

}


void QTextDrawer::Draw(QPainter& painter)
{
	painter.save();
	if (!m_strText.isEmpty()) {
		QFont font;
		font.setPointSize(m_property.size.PenSizeInt);
		font.setFamily("Microsoft YaHei");
		painter.setFont(font);
		QPen pen;
		pen.setColor(QColor(m_property.color));
		painter.setPen(pen);
		painter.setRenderHint(QPainter::Antialiasing);
		painter.drawText(m_posRect, m_strText);
	}
	painter.restore();
}