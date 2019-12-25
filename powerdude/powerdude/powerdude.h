#pragma once

#include <QtWidgets/QWidget>
#include "ui_powerdude.h"
#include "remotedesktopdialog.h"

class Powerdude : public QWidget
{
	Q_OBJECT

public:
	Powerdude(QWidget *parent = Q_NULLPTR);
	QString GetVersion(void);

private:
	Ui::Powerdude ui;

	MstscRdpInfo mMstscRdpInfo;

	QString version="0.1";

private slots:
	// connect system
	void connectRemoteSystem();

	//close connection
	void onAboutToClose();

protected:
	void execMessageBox(const QString & title, const QString & text);
	bool validInputInfo();
	void setConnectInfo();
	void saveCurrentConnectInfo();
	void readConnectIniInfo();
};
