#include "QDrawerPanel.h"

QDrawerPanel::QDrawerPanel(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	this->setMouseTracking(true);
}

QDrawerPanel::~QDrawerPanel()
{

}

void QDrawerPanel::mouseMoveEvent(QMouseEvent *pMouseMoveEvent)
{
	setCursor(Qt::ArrowCursor);
}