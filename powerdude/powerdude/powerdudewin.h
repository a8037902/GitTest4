#pragma once

#include <QWidget>
#include <QWebEngineView>
#include <QAxWidget> 
#include <QAxObject>
#include <QCloseEvent>

#include <QString>
#include <QSize>
#include "ui_powerdudewin.h"

class PowerdudeWin :
	public QWebEngineView
{
	Q_OBJECT
public:
	PowerdudeWin(QWidget *parent = Q_NULLPTR);

	//连接远程系统
	void connectRemoteSystem();
	//取消连接远程系统
	void disconnectRemoteSystem();

signals:
	void aboutToClose();

protected:
	//close current widget event
	void closeEvent(QCloseEvent *event);

private:
	Ui::PowerdudeWin ui;

	QAxWidget *mRemoteSystemView;
};

