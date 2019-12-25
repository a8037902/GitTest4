#pragma once
#include <QAxWidget>
#include <QMap>
#include <QWebEngineView>
#include <QWebEnginePage>
#include "MstscRdpInfo.h"
#include "ThumbWidget.h"
#include <qscrollarea.h>
#include <QListWidget>
#include <qscrollbar.h>
#include "onecloudwidget.h"
#include "logindatahandler.h"

class AxWidgetManage
	:public QObject
{
	Q_OBJECT
public:
	explicit AxWidgetManage();
	~AxWidgetManage();

	void frontWidgetShow(QWidget* pWidget);

	QString openAxWidget(QString &key, MstscRdpInfo & info, QIcon &icon);
	void openAxWidget(QString& key);
	void setConnectInfo(QAxWidget*,MstscRdpInfo &mstscRdpInfo);
	
	void disconnectAxWidget(QString& key);
	void connectAxWidget(QString& key);
	void closeAxWidget(QString& key);


	void addAxWidget(QString &name);
	QAxWidget* getAxWidget(QString &name);

	void setAxWidgetContainer(QWidget* pAxWidgetContainer) {
		this->pAxWidgetContainer = pAxWidgetContainer;
	}

	QWidget* getAxWidgetContainer() {
		return this->pAxWidgetContainer;
	}

	/*QAxWidget* getFrontWidget()
	{
		return pFrontWidget;
	}*/

	ThumbWidget* openWebEngineView(QString & key, QString& url, QIcon &icon);
	void openWebEngineView(QString & key);
	void closeWebEngineView(QString& key);
	void setUrl(QString & key, QString & url);

	/*QWebEngineView* getFrontWebEngineView()
	{
		return pFrontWebEngineView;
	}*/

	void changeWidget(int index) {
		/*if (pFrontWidget != NULL) {
			for (int i = 0;i < widgetList.size();i++) {
				index = (widgetList.indexOf(pFrontWidget) + 1 + i) % widgetList.size();
				if (!widgetList[index]->isHidden()) {
					frontWidgetShow(widgetList[index]);
					break;
				}
			}
		}*/

		if (widgetList.size() > index) {
			frontWidgetShow(widgetList[index]);
		}

		/*QList<QWidget*> list = pAxWidgetContainer->findChildren<QWidget*>();
		QList<QWidget*>::ConstIterator ci;
		for (ci = list.constBegin();ci != list.constEnd();++ci) {
			if ((*ci)->isActiveWindow()) {
				frontWidgetShow(*++ci);
				break;
			}
		}*/
	}

	void downWidget() {
		int index = 0;
		if (pFrontWidget != NULL) {
			pFrontWidget->hide();
			for (int i = 0;i < widgetList.size();i++) {
				index = (widgetList.indexOf(pFrontWidget) + widgetList.size() - 1 +i)% widgetList.size();
				if (!widgetList[index]->isHidden()) {
					frontWidgetShow(widgetList[index]);
					break;
				}
			}
		}
	}

	QList<QWidget*>& getWidgetList() {
		return widgetList;
	}

	int getNextWidget() {
		int index = 0;
		if (pFrontWidget != NULL) {
			index = (widgetList.indexOf(pFrontWidget) + widgetList.size()-1) % widgetList.size();
		}
		return index;
	}

	void clearWidget() {
		QMap<QString, QWidget*>::const_iterator i;
		for (i = widgetMap.constBegin(); i != widgetMap.constEnd(); ++i) {
			QWidget *pWidget = i.value();
			/*if (pWidget->inherits("OneCloudWidget")) {
				this->closeAxWidget(pWidget->whatsThis());
			}
			else {
				this->closeWebEngineView(pWidget->whatsThis());
			}*/
			pWidget->deleteLater();
		}
		widgetList.clear();
		widgetMap.clear();
		pFrontWidget = NULL;
	}

	bool contains(QString& key)
	{
		return widgetMap.contains(key);
	}

	OneCloudWidget* GetOneCloudWidget(QString& key) {
		OneCloudWidget* pWidget = 0;
		if (!widgetMap.contains(key)) {
			return pWidget;
		}
		pWidget = qobject_cast<OneCloudWidget*>(widgetMap.find(key).value());
		return pWidget;
	}

	QWebEngineView* GetWenEngineView(QString& key) {
		QWebEngineView* pWidget = 0;
		if (!widgetMap.contains(key)) {
			return pWidget;
		}
		pWidget = qobject_cast<QWebEngineView*>(widgetMap.find(key).value());
		return pWidget;
	}

	void closeFrontWidget();
	QString getFrontWidgetKey();

signals:
	void loginComplete(QString&);
	void disconnected(QString&);
	void toConnect(QString&);
	void toClose(QString&);
	void toOpen(QString&);
private slots:
	//axwidget event
	void onLoginComplete();
	void onDisconnected(int discReason);
	void onAuthenticationWarningDismissed();
	void onAuthenticationWarningDisplayed();
	void onLogonError(int error);
	void onNetworkStatusChanged(uint qualityLevel, int  bandwidth, int  rtt);

	void onConnected(QString&);
	void onOpened(QString&);

	void handleFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature);
	void downloadRequested(QWebEngineDownloadItem*);
	void onClose();
private:
	QWidget* pAxWidgetContainer;
	QWidget* pThumbsPanel;
	//QVBoxLayout*	pThumbsLayout;
	QListWidget*    pListWidget;
	//QAxWidget* pFrontAxWidget;
	//QWebEngineView* pFrontWebEngineView;
	QWidget* pFrontWidget;
	//QMap<QString, QAxWidget*> axwidgetMap;
	//QMap<QString, QWebEngineView*> webEngineViewMap;
	QMap<QString, QWidget*> widgetMap;
	//QMap<QString, ThumbWidget*> thumbMap;
	//QList<ThumbWidget*> thumbList;
	QList<QWidget*> widgetList;

	//int colorDepth;
	bool redirectDrives;
	bool redirectDevices;
	bool redirectPrinters;
	bool redirectPorts;

    LoginDataHandler *m_pLoginDataHandler;
};

