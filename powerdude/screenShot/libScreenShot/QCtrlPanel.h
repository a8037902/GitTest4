#pragma once

#include <QWidget>
#include <QMouseEvent>
#include "ui_QCtrlPanel.h"

class QCtrlPanel : public QWidget
{
	Q_OBJECT

public:
	QCtrlPanel(QWidget *parent = Q_NULLPTR);
	~QCtrlPanel();

	void displayResolution(int iResWidth, int iResHeight);
	void mouseMoveEvent(QMouseEvent *pMouseMoveEvent);
public:
	Ui::QCtrlPanel ui;
};
