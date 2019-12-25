#pragma once

#include <QObject>
#include "IDrawable.h"

class QTextDrawer : public IDrawable
{
	Q_OBJECT

public:
	QTextDrawer(QRect posRect, DrawableProperty pro, QString strText, QObject *parent = 0);
	~QTextDrawer();
	void Draw(QPainter& painter);
protected:
	QRect m_posRect;
	QString m_strText;
};
