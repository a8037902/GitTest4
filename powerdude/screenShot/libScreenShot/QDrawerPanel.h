#pragma once

#include <QWidget>
#include "ui_QDrawerPanel.h"
#include <QMouseEvent>

class QDrawerPanel : public QWidget
{
	Q_OBJECT

public:
	QDrawerPanel(QWidget *parent = Q_NULLPTR);
	~QDrawerPanel();
	void mouseMoveEvent(QMouseEvent *pMouseMoveEvent);
	Ui::QDrawerPanel ui;
};
