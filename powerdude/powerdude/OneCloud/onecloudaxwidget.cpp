#include "onecloudaxwidget.h"
#include <qDebug>
#include <QAxObject>
#include "translucentbutton.h"
/*dengpy*/
#include <rdpappcontainerclient.h>
#include <qScreen>
#include <qWindow>
/*dengpy*/

namespace OneCloud {
	OneCloudAxWidget::OneCloudAxWidget(OneCloudData *pData, QWidget *parent)
		: OneCloudView(pData, parent), pAxWidget(NULL)
	{

	}

	OneCloudAxWidget::OneCloudAxWidget(QWidget *parent)
		: OneCloudView(parent), pAxWidget(NULL)
	{
		
	}


	OneCloudAxWidget::~OneCloudAxWidget()
	{
		disconnect(pAxWidget, SIGNAL(OnConnected()), this, SLOT(onConnected()));
		disconnect(pAxWidget, SIGNAL(OnDisconnected(int)), this, SLOT(onDisconnected(int)));
	}

	void OneCloudAxWidget::init(QWidget * pOneCloudContainer)
	{
		pAxWidget = new QAxWidget(pOneCloudContainer);
		pAxWidget->setControl("{54d38bf7-b1ef-4479-9674-1bd6ea465258}");
		//pAxWidget->setWindowFlags(Qt::FramelessWindowHint);
		//pAxWidget->setStyleSheet("QGraphicsView{border:0px;}");
		//pAxWidget->setAutoFillBackground(false);
		pAxWidget->setProperty("geometry", QVariant(QRect(QPoint(0, 0), pOneCloudContainer->size())));
		pAxWidget->raise();
		pAxWidget->hide();

		connect(pAxWidget, SIGNAL(OnConnected()), this, SLOT(onConnected()));
		connect(pAxWidget, SIGNAL(OnDisconnected(int)), this, SLOT(onDisconnected(int)));
	}

	void OneCloudAxWidget::init(QWidget * pOneCloudContainer, QRect & rect)
	{
		pAxWidget = new QAxWidget(pOneCloudContainer);
		pAxWidget->setControl("{54d38bf7-b1ef-4479-9674-1bd6ea465258}");
		//pAxWidget->setWindowFlags(Qt::FramelessWindowHint);
		//pAxWidget->setStyleSheet("QGraphicsView{border:0px;}");
		//pAxWidget->setAutoFillBackground(false);
		pAxWidget->setProperty("geometry", QVariant(rect));
		pAxWidget->raise();
		//pAxWidget->hide();

		connect(pAxWidget, SIGNAL(OnConnected()), this, SLOT(onConnected()));
		connect(pAxWidget, SIGNAL(OnDisconnected(int)), this, SLOT(onDisconnected(int)));
	}

	void OneCloudAxWidget::setHidden(bool hidden)
	{

	}

	void OneCloudAxWidget::show()
	{
		//pAxWidget->show();
		pAxWidget->raise();
	}

	void OneCloudAxWidget::hide()
	{
		//pAxWidget->hide();
		pAxWidget->lower();
	}

	void OneCloudAxWidget::disconnectHost()
	{
		pAxWidget->dynamicCall("Disconnect()");
	}

	void OneCloudAxWidget::connectHost()
	{
		setConnectInfo();
		pAxWidget->dynamicCall("Connect()");
	}

	void OneCloudAxWidget::setConnectInfo()
	{
		int colorDepth = getOneCloudData("colorDepth").toInt();
		int desktopWidth= getOneCloudData("desktopWidth").toInt();
		int desktopHeight= getOneCloudData("desktopHeight").toInt();

		QString fulladdress = getOneCloudData("ip").toString();//"192.168.1.112";
		QString domain = getOneCloudData("domain").toString();
		QString username= getOneCloudData("username").toString();//"admin";
		QString password= getOneCloudData("password").toString();//"engine";
		int port= getOneCloudData("port").toInt();//0;

		if (desktopWidth == 0)desktopWidth = pAxWidget->geometry().width();//pAxWidget->parentWidget()->size().width();
		if (desktopHeight == 0)desktopHeight = pAxWidget->geometry().height();//pAxWidget->parentWidget()->size().height();

		pAxWidget->dynamicCall("SetDesktopWidth(int)", desktopWidth);
		pAxWidget->dynamicCall("SetDesktopHeight(int)", desktopHeight);
		pAxWidget->dynamicCall("SetColorDepth(int)", colorDepth);

		pAxWidget->setProperty("Server", fulladdress);

		if (!domain.isEmpty()) {
			//ui->axWidget->setProperty("Domain", mstscRdpInfo.domain);
			pAxWidget->setProperty("UserName", username + "@" + domain);
		}
		else {
			pAxWidget->setProperty("UserName", username);
		}



		//mRemoteSystemView->setProperty("RedirectDynamicDevices", true);


		QAxObject *advancedSettings = pAxWidget->querySubObject("AdvancedSettings8");
		if (advancedSettings) {
			advancedSettings->dynamicCall("SetClearTextPassword(const QString&)", password);
			if (port > 0)advancedSettings->dynamicCall("SetRDPPort(int)", port);

			advancedSettings->dynamicCall("SetAuthenticationLevel(int)", 2);

			advancedSettings->dynamicCall("SetRedirectDrives(bool)", true);
			advancedSettings->dynamicCall("SetRedirectDevices(bool)", true);
			advancedSettings->dynamicCall("SetRedirectPrinters(bool)", true);
			advancedSettings->dynamicCall("SetRedirectPorts(bool)", true);
			advancedSettings->dynamicCall("SetRedirectPOSDevices(bool)", true);
			advancedSettings->dynamicCall("SetRedirectDirectX(bool)", true);
		}
	}

	void OneCloudAxWidget::onConnected()
	{
		qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
		emit connected();
	}

	void OneCloudAxWidget::onDisconnected(int discReason)
	{
		qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
		emit disconnected();
	}
}