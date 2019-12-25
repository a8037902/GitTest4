#pragma once
#include <qwidget.h>
class TextMovingWidget : public QWidget
{
	Q_OBJECT

public:
	TextMovingWidget(QWidget *parent = 0);
	~TextMovingWidget();
	void addText(const QString&);
protected:
	virtual void paintEvent(QPaintEvent*);
	inline int getDirection(const QString&);
private slots:
	void slot_UpdateTextGeometry();
private:
	QTimer *_timer;
	QList<QString> textList;
	int m_direction; //zero means left ,one means right;
	int m_x;
	int state;//0,Õ£¡Ù°£1£¨“∆∂Ø
	int cnt;
	int index;
};
