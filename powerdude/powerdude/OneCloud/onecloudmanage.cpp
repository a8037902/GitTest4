#include "onecloudmanage.h"
#include <qDebug>

namespace OneCloud {
	OneCloudManage::OneCloudManage()
	{

	}


	OneCloudManage::~OneCloudManage()
	{

	}

	void OneCloudManage::openOneCloudMainWindow(QString & key, QString & name, OneCloudView * pOneCloudView, QIcon &icon)
	{
		OneCloudMainWindow*pOneCloudMainWindow = NULL;
		if (oneCloudMap.contains(key)) {
			pOneCloudMainWindow = oneCloudMap.find(key).value();
		}else{
			pOneCloudMainWindow = new OneCloudMainWindow(pOneCloudView,pOneCloudContainer);

			connect(pOneCloudMainWindow, SIGNAL(connected(QString&)), this, SLOT(onConnected(QString&)));
			connect(pOneCloudMainWindow, SIGNAL(disconnected(QString&)), this, SLOT(onDisconnected(QString&)));
			connect(pOneCloudMainWindow, SIGNAL(toConnect(QString&)), this, SLOT(onToConnect(QString&)));

			pOneCloudMainWindow->setKey(key);
			pOneCloudMainWindow->setWhatsThis(name);
			pOneCloudMainWindow->setWindowIcon(icon);
			pOneCloudMainWindow->connectHost();
			oneCloudMap.insert(key, pOneCloudMainWindow);
			oneCloudList.push_back(pOneCloudMainWindow);

			emit toOpen(key);
		}

		frontWidgetShow(pOneCloudMainWindow);
		
	}

	void OneCloudManage::openOneCloudMainWindow(QString & key)
	{
		OneCloudMainWindow*pOneCloudMainWindow = NULL;
		if (oneCloudMap.contains(key)) {
			pOneCloudMainWindow = oneCloudMap.find(key).value();
			frontWidgetShow(pOneCloudMainWindow);
			//emit toOpen(key);
		}
	}

	void OneCloudManage::closeOneCloudMainWindow(QString & key)
	{
		OneCloudMainWindow*pOneCloudMainWindow = NULL;
		if (oneCloudMap.contains(key)) {
			pOneCloudMainWindow = oneCloudMap.find(key).value();
			closeOneCloudMainWindow(pOneCloudMainWindow);
		}
	}

	void OneCloudManage::closeOneCloudMainWindow(OneCloudMainWindow * pOneCloudMainWindow)
	{
		if (pOneCloudMainWindow) {
			if (pFrontOneCloudMainWindow == pOneCloudMainWindow) {
				changeWidget(getNextWidget());
			}
			disconnect(pOneCloudMainWindow, SIGNAL(connected(QString&)), this, SLOT(onConnected(QString&)));
			disconnect(pOneCloudMainWindow, SIGNAL(disconnected(QString&)), this, SLOT(onDisconnected(QString&)));
			disconnect(pOneCloudMainWindow, SIGNAL(toConnect(QString&)), this, SLOT(onToConnect(QString&)));

			oneCloudMap.remove(pOneCloudMainWindow->getKey());
			oneCloudList.removeOne(pOneCloudMainWindow);
			pOneCloudMainWindow->deleteLater();

			emit toClose(pOneCloudMainWindow->getKey());
		}
	}

	void OneCloudManage::closeFrontWindow()
	{
		if (pFrontOneCloudMainWindow != NULL) {
			closeOneCloudMainWindow(pFrontOneCloudMainWindow);
		}
	}

	void OneCloudManage::clear()
	{
		QList<OneCloudMainWindow*> list = oneCloudList;
		QList<OneCloudMainWindow*>::iterator i;
		foreach(OneCloudMainWindow* p, list) {
			closeOneCloudMainWindow(p);
		}
	}

	void OneCloudManage::disconnectHost(QString & key)
	{
		OneCloudMainWindow*pOneCloudMainWindow = NULL;
		if (oneCloudMap.contains(key)) {
			pOneCloudMainWindow = oneCloudMap.find(key).value();
			pOneCloudMainWindow->disconnectHost();
		}
	}

	void OneCloudManage::connectHost(QString & key)
	{
		OneCloudMainWindow*pOneCloudMainWindow = NULL;
		if (oneCloudMap.contains(key)) {
			pOneCloudMainWindow = oneCloudMap.find(key).value();
			pOneCloudMainWindow->connectHost();
		}
	}

	

	void OneCloudManage::frontWidgetShow(OneCloudMainWindow*pOneCloudMainWindow) {
		//if (pFrontWidget != NULL)pFrontWidget->hide();
		pFrontOneCloudMainWindow = pOneCloudMainWindow;
		pFrontOneCloudMainWindow->show();
		//pFrontWidget->update();
		//pFrontWidget->activateWindow();
		pFrontOneCloudMainWindow->raise();

		oneCloudList.removeOne(pFrontOneCloudMainWindow);
		oneCloudList.push_back(pFrontOneCloudMainWindow);
	}

	bool OneCloudManage::contains(QString & key)
	{
		return oneCloudMap.contains(key);
	}


	void OneCloudManage::onConnected(QString&key)
	{
		emit connected(key);
	}

	void OneCloudManage::onDisconnected(QString&key)
	{
		emit disconnected(key);
	}
	void OneCloud::OneCloudManage::onToConnect(QString &key)
	{
		emit toConnect(key);
	}
}

