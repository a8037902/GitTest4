#include "axwidgetmanage.h"
#include <QAxObject>
#include <QWebEngineProfile>
#include <QDebug>
#include <QMessageBox>
#include <QDesktopServices>
#include <QUrl>
#include "onecloudwidget.h"
#include <Windows.h>
#include "Settings.h"
#include "onecloudmessagebox.h"


const int ColorDepth[] = {
	32,24,16,15
};

const int DesktopSize[][2] = {
	{0,0},
	{1440,900},
	{1366,768},
	{1280,800},
};



AxWidgetManage::AxWidgetManage()
{
	//colorDepth = 32;
	redirectDrives = true;
	redirectDevices = true;
	redirectPrinters = true;
	redirectPorts = true;

	//pFrontAxWidget = NULL;
	//pFrontWebEngineView = NULL;
	pFrontWidget = NULL;
	pThumbsPanel = 0;
	//pThumbsLayout = 0;
	pListWidget = 0;

    m_pLoginDataHandler = new LoginDataHandler();
}


AxWidgetManage::~AxWidgetManage()
{

}

void AxWidgetManage::frontWidgetShow(QWidget* pWidget) {
	//if (pFrontWidget != NULL)pFrontWidget->hide();
	pFrontWidget = pWidget;
	pFrontWidget->show();
	//pFrontWidget->update();
	//pFrontWidget->activateWindow();
	pFrontWidget->raise();

	widgetList.removeOne(pFrontWidget);
	widgetList.push_back(pFrontWidget);
}

QString AxWidgetManage::openAxWidget(QString &key,MstscRdpInfo & info, QIcon &icon)
{
	QAxWidget*pAxWidget = NULL;
	OneCloudWidget * pOneCloudWidget = NULL;
	if (widgetMap.contains(key)) {
		pOneCloudWidget = qobject_cast<OneCloudWidget*>(widgetMap.find(key).value());

        if (info.desktopType == RDPDestkopType::P2PDesktop){
            pAxWidget = pOneCloudWidget->getAxWidget();
            setConnectInfo(pAxWidget, info);
        }

		pOneCloudWidget->Connect();
	}
	else {
		SingleTon<Settings>::getInstance().beginGroup("RemoteDesktop");
		int desktopSize = SingleTon<Settings>::getInstance().value("desktopSize", 0).toInt();
		SingleTon<Settings>::getInstance().endGroup();
		int desktopWidth = DesktopSize[desktopSize][0];
		int desktopHeight = DesktopSize[desktopSize][1];
		if (desktopWidth == 0 || desktopWidth > pAxWidgetContainer->size().width())desktopWidth = pAxWidgetContainer->size().width();
		if (desktopHeight == 0 || desktopHeight > pAxWidgetContainer->size().height())desktopHeight = pAxWidgetContainer->size().height();

		pOneCloudWidget = new OneCloudWidget(desktopWidth, desktopHeight,pAxWidgetContainer);
		pAxWidget = pOneCloudWidget->getAxWidget();
		//pAxWidget->setWindowFlags(Qt::FramelessWindowHint);
		//pAxWidget->setStyleSheet("QGraphicsView{border:0px;}");
		//pAxWidget->setAutoFillBackground(false);
		//pAxWidget->setProperty("geometry", QVariant(QRect(QPoint(0, 0), pAxWidgetContainer->size())));
		pOneCloudWidget->setWhatsThis(key);
		pOneCloudWidget->setWindowIcon(icon);
		//pAxWidget->setWhatsThis(key);
		//pAxWidget->setWindowIcon(icon);
		//pAxWidgetContainer->update();
		if (!info.filepath.isEmpty()) {
			pOneCloudWidget->setFilePath(info.filepath);
		}

		widgetMap.insert(key, pOneCloudWidget);
		widgetList.push_back(pOneCloudWidget);

        if (info.desktopType == RDPDestkopType::P2PDesktop) {
            m_pLoginDataHandler->getP2PDesktopIpPort(info);
        }

        if (info.desktopType == RDPDestkopType::USBDesktop) {
            if (!m_pLoginDataHandler->usbAuth(info)) {
                return QString("");
            }
        }

		setConnectInfo(pAxWidget, info);

		connect(pOneCloudWidget, SIGNAL(OnLoginComplete()), this, SLOT(onLoginComplete()));
		connect(pOneCloudWidget, SIGNAL(OnDisconnected(int)), this, SLOT(onDisconnected(int)));
		connect(pOneCloudWidget, SIGNAL(OnAuthenticationWarningDismissed()), this, SLOT(onAuthenticationWarningDismissed()));
		connect(pOneCloudWidget, SIGNAL(OnAuthenticationWarningDisplayed()), this, SLOT(onAuthenticationWarningDisplayed()));
		connect(pOneCloudWidget, SIGNAL(OnLogonError(int)), this, SLOT(onLogonError(int)));
		connect(pOneCloudWidget, SIGNAL(OnNetworkStatusChanged(uint, int, int)), this, SLOT(onNetworkStatusChanged(uint, int, int)));

		connect(pOneCloudWidget, SIGNAL(toConnect(QString &)), this, SLOT(onConnected(QString &)));

		pOneCloudWidget->Connect();
	}
	//if(pFrontWebEngineView!=NULL)pFrontWebEngineView->hide();
	//if(pFrontAxWidget!=NULL)pFrontAxWidget->hide();
	frontWidgetShow(pOneCloudWidget);
	return key;
}

void AxWidgetManage::openAxWidget(QString & key)
{
	OneCloudWidget* pOneCloudWidget;
	if (widgetMap.contains(key)) {
		pOneCloudWidget = qobject_cast<OneCloudWidget*>(widgetMap.find(key).value());
		pOneCloudWidget->Connect();
		frontWidgetShow(pOneCloudWidget);
	}
}

void AxWidgetManage::setConnectInfo(QAxWidget*pAxWidget,MstscRdpInfo &mstscRdpInfo)
{
	//MstscRdpInfo mstscRdpInfo;
	//mstscRdpInfo.fulladdress = "192.168.1.112";
	//mstscRdpInfo.username = "admin";// "tester";
	//mstscRdpInfo.password = "engine";
	//int index = ui->mDesktopSize->currentIndex();
	//mstscRdpInfo.desktopsize = QSize(1024, 768);
	SingleTon<Settings>::getInstance().beginGroup("RemoteDesktop");
	int colorDepth = ColorDepth[SingleTon<Settings>::getInstance().value("colorDepth",0).toInt()];
	int desktopSize = SingleTon<Settings>::getInstance().value("desktopSize",0).toInt();
	SingleTon<Settings>::getInstance().endGroup();
	int desktopWidth = DesktopSize[desktopSize][0];
	int desktopHeight = DesktopSize[desktopSize][1];
	if (desktopWidth == 0)desktopWidth = pAxWidgetContainer->size().width();
	if (desktopHeight == 0)desktopHeight = pAxWidgetContainer->size().height();
	
	pAxWidget->dynamicCall("SetDesktopWidth(int)", desktopWidth);
	pAxWidget->dynamicCall("SetDesktopHeight(int)", desktopHeight);
	pAxWidget->dynamicCall("SetColorDepth(int)", colorDepth);
	//pAxWidget->dynamicCall("SetShowRedirectionWarningDialog(bool)", true);

	qDebug() << pAxWidgetContainer->size().width() << pAxWidgetContainer->size().height();

	//ui->axWidget->dynamicCall("SetRedirectDynamicDevices(bool)", true);
	//ui->axWidget->dynamicCall("SetRedirectDynamicDrives(bool)", true);
	//ui->axWidget->dynamicCall("SetConnectionBarText(const QString&)", "test");
	//ui->axWidget->dynamicCall("SetShowRedirectionWarningDialog(bool)", true);
	//ui->axWidget->dynamicCall("SetWarnAboutClipboardRedirection(bool)", true);

	//ui->axWidget->setFixedSize(mstscRdpInfo.desktopsize);

	pAxWidget->setProperty("Server", mstscRdpInfo.fulladdress);

	if (!mstscRdpInfo.domain.isEmpty()) {
		//ui->axWidget->setProperty("Domain", mstscRdpInfo.domain);
		pAxWidget->setProperty("UserName", mstscRdpInfo.username + "@" + mstscRdpInfo.domain);
	}
	else {
		pAxWidget->setProperty("UserName", mstscRdpInfo.username);
	}



	//mRemoteSystemView->setProperty("RedirectDynamicDevices", true);


	QAxObject *advancedSettings = pAxWidget->querySubObject("AdvancedSettings8");
	if (advancedSettings) {
		advancedSettings->dynamicCall("SetClearTextPassword(const QString&)", mstscRdpInfo.password);
		if (mstscRdpInfo.port > 0)advancedSettings->dynamicCall("SetRDPPort(int)", mstscRdpInfo.port);

		advancedSettings->dynamicCall("SetAuthenticationLevel(int)", 2);

		advancedSettings->dynamicCall("SetRedirectDrives(bool)", redirectDrives);
		advancedSettings->dynamicCall("SetRedirectDevices(bool)", redirectDevices);
		advancedSettings->dynamicCall("SetRedirectPrinters(bool)", redirectPrinters);
		advancedSettings->dynamicCall("SetRedirectPorts(bool)", redirectPorts);
		advancedSettings->dynamicCall("SetRedirectPOSDevices(bool)", true);
		advancedSettings->dynamicCall("SetRedirectDirectX(bool)", true);
	}
}

void AxWidgetManage::disconnectAxWidget(QString & key)
{
	OneCloudWidget*pOneCloudWidget = NULL;
	if (widgetMap.contains(key)) {
		pOneCloudWidget = qobject_cast<OneCloudWidget*>(widgetMap.find(key).value());
		pOneCloudWidget->Disconnect();
		//pAxWidget->hide();
	}
}

void AxWidgetManage::connectAxWidget(QString & key)
{
	OneCloudWidget*pOneCloudWidget = NULL;
	if (widgetMap.contains(key)) {
		pOneCloudWidget = qobject_cast<OneCloudWidget*>(widgetMap.find(key).value());
		pOneCloudWidget->Connect();
		//pAxWidget->show();
	}
}

void AxWidgetManage::closeAxWidget(QString & key)
{
	//QAxWidget*pAxWidget = NULL;
	OneCloudWidget *pOneCloudWidget = NULL;
	if (widgetMap.contains(key)) {
		pOneCloudWidget = qobject_cast<OneCloudWidget*>(widgetMap.find(key).value());
		//pAxWidget = pOneCloudWidget->getAxWidget();
		if (pFrontWidget == pOneCloudWidget) {
			changeWidget(getNextWidget());
		}

		disconnect(pOneCloudWidget, SIGNAL(OnLoginComplete()), this, SLOT(onLoginComplete()));
		disconnect(pOneCloudWidget, SIGNAL(OnDisconnected(int)), this, SLOT(onDisconnected(int)));
		disconnect(pOneCloudWidget, SIGNAL(OnAuthenticationWarningDismissed()), this, SLOT(onAuthenticationWarningDismissed()));
		disconnect(pOneCloudWidget, SIGNAL(OnAuthenticationWarningDisplayed()), this, SLOT(onAuthenticationWarningDisplayed()));
		disconnect(pOneCloudWidget, SIGNAL(OnLogonError(int)), this, SLOT(onLogonError(int)));
		disconnect(pOneCloudWidget, SIGNAL(OnNetworkStatusChanged(uint, int, int)), this, SLOT(onNetworkStatusChanged(uint, int, int)));

		disconnect(pOneCloudWidget, SIGNAL(toConnect(QString &)), this, SLOT(onConnected(QString &)));
		emit disconnected(key);
		widgetMap.remove(key);
		widgetList.removeOne(pOneCloudWidget);
		pOneCloudWidget->deleteLater();

		if (widgetMap.size() == 0) {
			pFrontWidget = NULL;
		}

		/*if (pFrontWebEngineView != NULL)pFrontWebEngineView->hide();
		if (pFrontAxWidget != NULL)pFrontAxWidget->hide();
		pFrontAxWidget = pAxWidget;
		pFrontAxWidget->show();*/
		//pFrontWidget = NULL;
	}
}

void AxWidgetManage::onClose()
{
	ThumbWidget* thumb = static_cast<ThumbWidget*>(sender());
	emit toClose(thumb->whatsThis());
}

void AxWidgetManage::addAxWidget(QString & name)
{
	/*if (!axwidgetMap.contains(name)) {
		axwidgetMap.insert(name, new QAxWidget());
	}*/
}

QAxWidget* AxWidgetManage::getAxWidget(QString & name)
{
	/*QAxWidget* pAxWidget=NULL;
	if (axwidgetMap.contains(name)) {
		pAxWidget = axwidgetMap.find(name).value();
	}

	return pAxWidget;*/
	return NULL;
}

void AxWidgetManage::onLoginComplete()
{
	//qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	OneCloudWidget *pOneCloudWidget = qobject_cast<OneCloudWidget *>(sender());
	emit loginComplete(pOneCloudWidget->whatsThis());
}

ThumbWidget* AxWidgetManage::openWebEngineView(QString & key,QString & url,QIcon &icon)
{
	//QString key = url;
	QWebEngineView *pWebEngineView = NULL;
	ThumbWidget* thumb = 0;
	if (widgetMap.contains(key)) {
		pWebEngineView = qobject_cast<QWebEngineView*>(widgetMap.find(key).value());
		//pWebEngineView->setUrl(url);
	}
	else {
		QWebEngineProfile *pWebEngineProfile = new QWebEngineProfile();
		pWebEngineProfile->setDownloadPath(SingleTon<Settings>::getInstance().oneValue("File", "filePath", QStandardPaths::standardLocations(QStandardPaths::DownloadLocation)).toString());
		QWebEnginePage *pWebEnginePage = new QWebEnginePage(pWebEngineProfile);

		pWebEngineView = new QWebEngineView(pAxWidgetContainer);
		//pWebEngineView->setObjectName(QString::fromUtf8("webEngineView"));
		pWebEngineView->setGeometry(QRect(QPoint(0, 0), pAxWidgetContainer->size()));
		pWebEngineView->setPage(pWebEnginePage);
		//QPalette palette;
		//QPixmap bgImage(":/powerdudemain/Resources/u9.jpeg");
		//bgImage.scaled(pAxWidgetContainer->size());//, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
		//palette.setBrush(QPalette::Background, QBrush(bgImage.scaled(pAxWidgetContainer->size())));
		//pWebEngineView->setPalette(palette);
		//pWebEngineView->setAutoFillBackground(true);

		//pWebEngineView->setAutoFillBackground(false);
		pWebEngineView->setStyleSheet(QString::fromUtf8(""));
		pWebEngineView->setUrl(QUrl(url));
		pWebEngineView->setWhatsThis(key);
		pWebEngineView->setWindowIcon(icon);
		widgetMap.insert(key, pWebEngineView);
		widgetList.push_back(pWebEngineView);
		thumb = new ThumbWidget(pWebEngineView);
		thumb->setWhatsThis(key);

		QWebEnginePage * page = pWebEngineView->page();
		connect(page, &QWebEnginePage::featurePermissionRequested, this, &AxWidgetManage::handleFeaturePermissionRequested);
		connect(page->profile(), SIGNAL(downloadRequested(QWebEngineDownloadItem*)), this, SLOT(downloadRequested(QWebEngineDownloadItem*)));
	}
	//if(pFrontAxWidget!=NULL)pFrontAxWidget->hide();
	//if(pFrontWebEngineView!=NULL)pFrontWebEngineView->hide();
	frontWidgetShow(pWebEngineView);
	return thumb;
}

void AxWidgetManage::openWebEngineView(QString & key)
{
	QWebEngineView* pWebEngineView;
	if (widgetMap.contains(key)) {
		pWebEngineView = qobject_cast<QWebEngineView*>(widgetMap.find(key).value());
		frontWidgetShow(pWebEngineView);
	}
}

void AxWidgetManage::closeWebEngineView(QString & key)
{
	QWebEngineView *pWebEngineView = NULL;
	if (widgetMap.contains(key)) {
		pWebEngineView = qobject_cast<QWebEngineView*>(widgetMap.find(key).value());
		if (pFrontWidget == pWebEngineView) {
			changeWidget(getNextWidget());
		}
		widgetMap.remove(key);
		widgetList.removeOne(pWebEngineView);
		pWebEngineView->deleteLater();

		if (widgetMap.size() == 0) {
			pFrontWidget = NULL;
		}
	}
}

void AxWidgetManage::setUrl(QString & key, QString & url)
{
	QWebEngineView* pWebEngineView;
	if (widgetMap.contains(key)) {
		pWebEngineView = qobject_cast<QWebEngineView*>(widgetMap.find(key).value());
		pWebEngineView->setUrl(url);
	}
}

void AxWidgetManage::onDisconnected(int discReason)
{
	//qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;

	/*isConnect = false;
	ui->widget_2->show();
	ui->widget->show();
	ui->axWidget->hide();*/
	//QAxWidget *pAxWidget = qobject_cast<QAxWidget *>(sender());
	OneCloudWidget *pOneCloudWidget = qobject_cast<OneCloudWidget *>(sender());
	emit disconnected(pOneCloudWidget->whatsThis());
}


void AxWidgetManage::onAuthenticationWarningDismissed()
{
	//qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
}

void AxWidgetManage::onAuthenticationWarningDisplayed()
{
	//qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
}

void AxWidgetManage::onLogonError(int error)
{
	//qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
}

void AxWidgetManage::onNetworkStatusChanged(uint qualityLevel, int bandwidth, int rtt)
{
	//qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	//qDebug() << "qualityLevel:" << qualityLevel << "bandwidth:" << bandwidth << "rtt:" << rtt;
}

void AxWidgetManage::onConnected(QString &key)
{
	emit toConnect(key);
}

void AxWidgetManage::onOpened(QString& key)
{
	emit toOpen(key);
}

inline QString questionForFeature(QWebEnginePage::Feature feature)
{
	switch (feature) {
	case QWebEnginePage::Geolocation:
		return QWebEnginePage::tr("Allow %1 to access your location information?");
	case QWebEnginePage::MediaAudioCapture:
		return QWebEnginePage::tr("Allow %1 to access your microphone?");
	case QWebEnginePage::MediaVideoCapture:
		return QWebEnginePage::tr("Allow %1 to access your webcam?");
	case QWebEnginePage::MediaAudioVideoCapture:
		return QWebEnginePage::tr("Allow %1 to access your microphone and webcam?");
	case QWebEnginePage::MouseLock:
		return QWebEnginePage::tr("Allow %1 to lock your mouse cursor?");
	case QWebEnginePage::DesktopVideoCapture:
		return QWebEnginePage::tr("Allow %1 to capture video of your desktop?");
	case QWebEnginePage::DesktopAudioVideoCapture:
		return QWebEnginePage::tr("Allow %1 to capture audio and video of your desktop?");
	case QWebEnginePage::Notifications:
		return QWebEnginePage::tr("Allow %1 to show notification on your desktop?");
	}
	return QString();
}

void AxWidgetManage::handleFeaturePermissionRequested(const QUrl &securityOrigin, QWebEnginePage::Feature feature)
{
	QString title = tr("Permission Request");
	QWebEnginePage *pPage = qobject_cast<QWebEnginePage *>(sender());
	/*QString question = questionForFeature(feature).arg(securityOrigin.host());
	if (!question.isEmpty() && QMessageBox::question(pPage->view()->window(), title, question) == QMessageBox::Yes)
		pPage->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);
	else
		pPage->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionDeniedByUser);*/
	pPage->setFeaturePermission(securityOrigin, feature, QWebEnginePage::PermissionGrantedByUser);

}

void AxWidgetManage::downloadRequested(QWebEngineDownloadItem *item)
{
	//qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	//qDebug() << item->path() << item->url();

    connect(item, &QWebEngineDownloadItem::finished, [=] {
        bool isAutoOpen;
        SingleTon<Settings>::getInstance().beginGroup("File");
        isAutoOpen = SingleTon<Settings>::getInstance().value("fileOpen", false).toBool();
        SingleTon<Settings>::getInstance().endGroup();

        if (isAutoOpen) {
            if (item->path().isEmpty())
                return;

            QString fileurl = QString("%1%2").arg("file:").arg(item->path());
            QDesktopServices::openUrl(fileurl);
        }
        else {
			OneCloudMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("下载完成！"));
        }
    });

	QString path = SingleTon<Settings>::getInstance().oneValue("File", "filePath", QStandardPaths::standardLocations(QStandardPaths::DownloadLocation)).toString();///*qApp->applicationDirPath() + "/" + */item->path();
	if (path.isEmpty())
		return;

	item->setPath(path);
	item->accept();
}

void AxWidgetManage::closeFrontWidget()
{
	//if(pFrontWidget.g)
}

QString AxWidgetManage::getFrontWidgetKey()
{
	if(pFrontWidget!=NULL)return pFrontWidget->whatsThis();
	return QString::null;
}
