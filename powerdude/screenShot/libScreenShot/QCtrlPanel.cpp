#include "QCtrlPanel.h"
#include <Windows.h>

QCtrlPanel::QCtrlPanel(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	setWindowFlags(Qt::FramelessWindowHint);
	this->setMouseTracking(true);
}

QCtrlPanel::~QCtrlPanel()
{

}

void QCtrlPanel::displayResolution(int iResWidth, int iResHeight)
{
	QString strText;
	strText.sprintf("%d x %d", iResWidth, iResHeight);
	ui.label_Resolution->setText(strText);
}

void QCtrlPanel::mouseMoveEvent(QMouseEvent *pMouseMoveEvent)
{
	setCursor(Qt::ArrowCursor);
}