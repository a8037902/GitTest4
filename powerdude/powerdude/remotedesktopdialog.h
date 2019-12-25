#pragma once

#include <QDialog>
#include "ui_remotedesktopdialog.h"
#include <QAxWidget> 
#include <QAxObject>
#include <QCloseEvent>

#include "MstscRdpInfo.h"



class RemoteDesktopDialog :
	public QDialog
{
	Q_OBJECT
public:
	RemoteDesktopDialog(const MstscRdpInfo &mstscRdpInfo, QWidget *parent = Q_NULLPTR);

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
	Ui::RemoteDesktopDialog ui;

	QAxWidget *mRemoteSystemView;
};

