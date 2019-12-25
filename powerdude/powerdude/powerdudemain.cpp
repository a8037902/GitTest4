#include "powerdudemain.h"
#include <QCryptographicHash>
#include <QListWidgetItem>
#include <QUuid>
#include <QApplication>
#include <QDesktopWidget>
#include <QAxObject>
#include <QShortcut>
#include <QScrollArea>
#include <QScrollBar>
#include <QMessageBox>
#include <QDesktopServices>
#include "lock.h"
//#include <qdebug.h>
//#include <QSslSocket>
#include "settings.h"
#include "hook.h"
#include "version.h"
#include "cloudblockmanager.h"
#include "mhook-lib/mhook.h"
#include "../screenShot/libScreenShot/libScreenShot.h"
#include <QBitmap>
#include "font.h"
#include "httpclient.h"
#include "OneCloudMessageBox.h"
#include "OneCloudDataFactory.h"
#include <Windows.h>

static pfBitBlt g_pfBitBlt = 0;
static PowerdudeMain* g_pThis = 0;

BOOL WINAPI BitBltProc(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop)
{
	BOOL hResult = g_pfBitBlt(hdc, x, y, cx, cy, hdcSrc, x1, y1, rop);
	HWND hWnd = ::WindowFromDC(hdc);
	if (hWnd == 0 || g_pThis == 0) {
		return hResult;
	}
	ThumbWidget* pWidget = g_pThis->getThumbWidget(hWnd);
	if (0 != pWidget) {
		HDC hCopyDC = pWidget->LockDC(hdc);
		g_pfBitBlt(hCopyDC, x, y, cx, cy, hdcSrc, x1, y1, rop);
		pWidget->UnlockDC();
	}
	pWidget = 0;
	if (g_pThis->m_pTabIconDialog != 0) {
		pWidget = g_pThis->m_pTabIconDialog->getThumbWidget(hWnd);
	}
	if (0 != pWidget) {
		HDC hCopyDC = pWidget->LockDC(hdc);
		g_pfBitBlt(hCopyDC, x, y, cx, cy, hdcSrc, x1, y1, rop);
		pWidget->UnlockDC();
	}
	return hResult;
}

const QString LIST_VIEW_TABLE[][5] = {
	{"7","",QStringLiteral(":/powerdudemain/Resources/聊天.png"),QStringLiteral("聊天"),"https://pdchat.pispower.com"},
	{"101","",QStringLiteral(":/powerdudemain/Resources/赞品.png"),QStringLiteral("赞品"),"https://store.onecloud.cn/shoppcweb"},
	{"14","",QStringLiteral(":/powerdudemain/Resources/亦云空间.png"),QStringLiteral("亦云空间"),"https://space.pispower.com"},
	{"2",QStringLiteral("云桌面列表"),QStringLiteral(":/powerdudemain/Resources/显示器.png"),QStringLiteral("云桌面"),""},
};

PowerdudeMain::PowerdudeMain(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::PowerdudeMain())
	, pbVncSshDesktop(new VncSshDesktop())
	, webSocket(new WebSocket())
    , secondScanWebSocket(new WebSocket())
	, cloudblkmgr(new CloudBlockManager())
    , m_pLoginDataHandler(new LoginDataHandler())
	, m_pListWidget(0)
	, m_pRdpsDockWidget(0)
	, m_pListWidgetSSH(0)
	, m_pSSHDockWidget(0)
	, m_pListWidgetVNC(0)
	, m_pVNCDockWidget(0)
	, m_pListWidgetShare(0)
	, m_pShareDockWidget(0)
	, m_pDockMenu(0)
	, m_pToolMenu(0)
	, m_pTimer(new QTimer(this))
	, waitHide(false)
	, m_pScreenRecordDialog(0)
	, m_pTabIconDialog(0)
	, m_pSetDialog(0)
	//, m_pSreenRecFrame(0)
	//,settings(new QSettings(qApp->applicationDirPath() + "\\settings.ini", QSettings::IniFormat))
	//, remoteDesktopNum(0)
{
	ui->setupUi(this);
	g_pThis = this;

	Qt::WindowFlags flags =
		(windowFlags());
	setWindowFlags(Qt::WindowType::FramelessWindowHint);

	showFullScreen();
	//this->setGeometry(0, 0, 1024, 768);
	//this->update();
	//this->resize(QSize(1024, 768));

	//webSocket = new WebSocket();
	//readIni();

	int verticalFrameW = 100;
	if (this->width() <= 1024) {

	}
	else if (this->width() <= 1440) {

	}
	else if (this->width() <= 1920) {

	}

	timer = new QTimer();
	//设置定时器每个多少毫秒发送一个timeout()信号  
	timer->setInterval(1*60000);
	//启动定时器  
	timer->start();
	connect(timer, SIGNAL(timeout()), this, SLOT(onLoginTimerOut()));
	//qDebug() << QSslSocket::supportsSsl() << QSslSocket::sslLibraryBuildVersionString() << QSslSocket::sslLibraryVersionString();

	//别忘了头文件
	//QShortcut *temp = new QShortcut(this);
	//设置键值，也就是设置快捷键.
	//temp->setKey(tr("alt+d"));
	//这个成员函数挺关键的，设置是否会自动反复按键.也就是说，当你一直按住键盘ctrl+r时，会一直不停的调用对应的槽函数.
	//temp->setAutoRepeat(false);
	//连接信号与槽，showSlot()是自定义的槽函数!
	//connect(temp, SIGNAL(activated()), this, SLOT(onAltAndD()));

	QPalette palette;
	QPixmap bgImage(":/powerdudemain/Resources/u8.jpg");
	bgImage.scaled(this->size());//, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
	palette.setBrush(QPalette::Background, QBrush(bgImage.scaled(this->size())));
	ui->widget_3->setPalette(palette);
	//ui->widget_3->setStyleSheet("background-image: url(:/powerdudemain/Resources/u8.png);");

	//ui->verticalFrame->setAutoHide(true);
	//ui->verticalFrame->setHideDriection(AutoHideFrame::Driection::Left);
	ui->verticalFrame->setGeometry(0, 0, verticalFrameW, this->size().height());
	ui->horizontalFrame->setGeometry(0 - verticalFrameW + 2, 0, verticalFrameW, this->size().height());
	ui->horizontalFrame->setAutoHideSize(QSize(verticalFrameW, this->size().height()));
	ui->horizontalFrame->setHideDriection(AutoHideFrame::Driection::Left);
	ui->horizontalFrame->setAutoHide(false);
	ui->horizontalFrame->setAttribute(Qt::WA_TranslucentBackground);


	//ui->listWidget->setGridSize(QSize(/*ui->verticalFrame->size().width()/2, ui->verticalFrame->size().width()/2*/95,95));
	//ui->listWidget->setIconSize(QSize(/*ui->verticalFrame->size().width()*4/5, ui->verticalFrame->size().width() * 4 / 5*/38,38));
	QFont font = ui->listWidget->font();
	font.setPixelSize(12);
	ui->listWidget->setFont(font);
	/*QListWidgetItem *item = ui->listWidget->item(0);
	item->setSizeHint(QSize(80, 80));*/

	ui->helpBtn->hide();
	//ui->lockBtn->hide();

	//ui->helpBtnFrame->setAutoHide(true);

	//ui->helpBtnFrame->setParent(ui->verticalFrame);

	infoListWidgetList.push_back(ui->remoteDesktopFrame);
	infoListWidgetList.push_back(ui->shareDesktopFrame);
	infoListWidgetList.push_back(ui->VNCFrame);
	infoListWidgetList.push_back(ui->SSHFrame);
	infoListWidgetList.push_back(ui->webFrame);
	infoListWidgetList.push_back(ui->rdpGroupFrame);
	infoListWidgetList.push_back(ui->fileFrame);
	ui->listWidget->item(3)->setData(100, 100);
	ui->listWidget->item(4)->setData(100, 101);
	ui->listWidget->item(5)->setData(100, 102);
	ui->listWidget->item(6)->setData(100, 103);
	ui->listWidget->item(8)->setData(100, 104);
	ui->listWidget->item(9)->setData(100, 105);
	ui->listWidget->item(10)->setData(100, 106);

	ui->listWidget->setFont(Font(12));
	/**deng*/
	ui->listWidget->setMouseTracking(true);
	connect(ui->listWidget, SIGNAL(itemEntered(QListWidgetItem *)), this, SLOT(onItemEntered(QListWidgetItem *)));
	/**deng*/


#ifndef ONE_CLOUD_USE
	connect(&axWidgetManage, SIGNAL(toOpen(QString &)), this, SLOT(onOpenAxWidget(QString &)));

	for (int i = 0;i < infoListWidgetList.size();i++) {
		infoListWidgetList[i]->setGeometry(verticalFrameW, 0, 420, this->size().height());
		infoListWidgetList[i]->hide();
		if (i == 0|| i == 6 || i == 5) {
			connect(infoListWidgetList[i], SIGNAL(openWidget(QString&)), this, SLOT(onOpenAxWidget(QString&)));
			connect(infoListWidgetList[i], SIGNAL(connectWidget(QString&)), this, SLOT(onConnectAxWidget(QString&)));
			connect(infoListWidgetList[i], SIGNAL(closeWidget(QString&)), this, SLOT(onCloseAxWidget(QString&)));
		}
		else {
			connect(infoListWidgetList[i], SIGNAL(openWidget(QString&)), this, SLOT(onOpenWebEngineView(QString&)));
			connect(infoListWidgetList[i], SIGNAL(connectWidget(QString&)), this, SLOT(onConnectWebEngineView(QString&)));
			connect(infoListWidgetList[i], SIGNAL(closeWidget(QString&)), this, SLOT(onCloseWebEngineView(QString&)));
		}
		connect(infoListWidgetList[i], SIGNAL(updateWidget()), this, SLOT(onUpdateWidget()));
	}
#else
	for (int i = 0;i < infoListWidgetList.size();i++) {
		infoListWidgetList[i]->setGeometry(verticalFrameW, 0, 420, this->size().height());
		infoListWidgetList[i]->hide();
	}
#endif

	//QScrollArea *pScroll = new QScrollArea(ui->remoteDesktopFrame);
	//pScroll->setWidget(ui->bodyFrame);
	//ui->verticalLayout_3->addWidget(pScroll);

	//ui->widget->setGeometry(QRect(this->size().width() / 2 - 20, this->size().height() / 2 - 20, 40, 40));
	//ui->widget_2->setGeometry(QRect(this->size().width()/2-122, this->size().height()/2-122,244,244));
	ui->loginWidgetFrame->setGeometry(QRect(this->size().width() / 2 - 300, this->size().height() / 2 - 300, 600, 420));

	//ui->webEngineView->setGeometry(QRect(QPoint(0, 0), this->size()));
	//ui->webEngineView->hide();
#ifdef ONE_CLOUD_USE
	oneCloudManage.setOneCloudContainer(ui->widgetFrame);
	connect(&oneCloudManage, SIGNAL(toOpen(QString &)), this, SLOT(onOneCloudWindowOpened(QString &)));
	connect(&oneCloudManage, SIGNAL(toClose(QString &)), this, SLOT(onOneCloudWindowCloseed(QString &)));
	connect(&oneCloudManage, SIGNAL(connected(QString &)), this, SLOT(onOneCloudWindowConnected(QString &)));
	connect(&oneCloudManage, SIGNAL(toConnect(QString &)), this, SLOT(onOneCloudWindowToConnect(QString &)));
	connect(&oneCloudManage, SIGNAL(disconnected(QString &)), this, SLOT(onOneCloudWindowDisconnected(QString &)));

	for (int i = 0;i < sizeof(LIST_VIEW_TABLE) / sizeof(LIST_VIEW_TABLE[0]);i++) {
		InfoListWidget* pInfoListWidget=NULL;
		if (!LIST_VIEW_TABLE[i][1].isEmpty()) {
			pInfoListWidget = new InfoListWidget(ui->horizontalFrame);
			//pInfoListWidget->setObjectName(QString::fromUtf8(LIST_VIEW_TABLE[i][0]));
			pInfoListWidget->setGeometry(verticalFrameW, 0, 420, this->size().height());
			ui->listWidget->sizePolicy().setHeightForWidth(pInfoListWidget->sizePolicy().hasHeightForWidth());
			pInfoListWidget->setSizePolicy(ui->listWidget->sizePolicy());
			pInfoListWidget->setBaseSize(QSize(0, 0));
			pInfoListWidget->setAutoFillBackground(false);
			pInfoListWidget->setStyleSheet(QString::fromUtf8("background-color: rgb(255, 255, 255);"));
			pInfoListWidget->setHasBtn(false);
			pInfoListWidget->setListName(LIST_VIEW_TABLE[i][1]);
			pInfoListWidget->hide();

			connect(pInfoListWidget, SIGNAL(openWidget(QString&)), this, SLOT(onOpenOneCloudWindow(QString&)));
			connect(pInfoListWidget, SIGNAL(connectWidget(QString&)), this, SLOT(onConnectOneCloudWindow(QString&)));
			connect(pInfoListWidget, SIGNAL(disconnectWidget(QString&)), this, SLOT(onDisconnectOneCloudWindow(QString&)));
			connect(pInfoListWidget, SIGNAL(closeWidget(QString&)), this, SLOT(onCloseOneCloudWindow(QString&)));
			connect(pInfoListWidget, SIGNAL(updateWidget()), this, SLOT(onUpdateWidget()));
		}
		else {
			QJsonObject jsonObject;
			jsonObject.insert("url", QJsonValue(LIST_VIEW_TABLE[i][4]));
			jsonObject.insert("key", QJsonValue(LIST_VIEW_TABLE[i][4]));
			jsonObject.insert("name", QJsonValue(LIST_VIEW_TABLE[i][4]));
			jsonObject.insert("downloadPath", SingleTon<Settings>::getInstance().oneValue("File", "filePath", QStandardPaths::standardLocations(QStandardPaths::DownloadLocation)).toString());
			OneCloudData* pOneCloudData = OneCloudDataFactory::getOneCloudDataFromJsonObject(LIST_VIEW_TABLE[i][0].toInt(), jsonObject);

			oneCloudDataMap.insert(LIST_VIEW_TABLE[i][4], pOneCloudData);
		}
		controlPanelInfos[i].pInfoListWidget = pInfoListWidget;
		controlPanelInfos[i].icon = QIcon(LIST_VIEW_TABLE[i][2]);
		controlPanelInfos[i].name = LIST_VIEW_TABLE[i][3];
		controlPanelInfos[i].key = LIST_VIEW_TABLE[i][4];

		controlPanelInfoMap.insert(LIST_VIEW_TABLE[i][0].toInt(), controlPanelInfos[i]);
	}

	
	


	listViewInit(QString("2,14,101"));
	connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(singleclicked2(QListWidgetItem*)));

	connect(webSocket, SIGNAL(oneCloudDataReceived(int, QJsonObject &)), this, SLOT(onOneCloudDataReceived(int, QJsonObject &)));
#endif

	axWidgetManage.setAxWidgetContainer(ui->widgetFrame);
	ui->widgetFrame->setGeometry(0, 0, this->size().width(), this->size().height());
	//ui->widget_4->hide();

	ui->textMovingWidget->addText(QStringLiteral("天空不曾留下飞鸟的痕迹，但我已飞过。"));
	ui->textMovingWidget->addText(QStringLiteral("书犹药也，善读可以医愚。"));
	ui->textMovingWidget->setGeometry(this->size().width() / 2 - 150, this->size().height() / 2 + 300, 300, 40);
	//ui->textMovingWidget->raise();

	/*ui->axWidget->setWindowFlags(Qt::FramelessWindowHint);
	ui->axWidget->setStyleSheet("QGraphicsView{border:0px;}");
	ui->axWidget->setProperty("geometry", QVariant(QRect(QPoint(0, 0), this->size())));
	ui->axWidget->hide();*/

	/*ui->axWidget_2->setProperty("geometry", QVariant(QRect(QPoint(0, 0), this->size())));
	ui->axWidget_2->hide();*/
	onValueChanged(0);
#ifndef ONE_CLOUD_USE
	connect(ui->listWidget, SIGNAL(itemClicked(QListWidgetItem*)), this, SLOT(singleclicked(QListWidgetItem*)));
#endif
	connect(ui->listWidget->verticalScrollBar(), SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));

	//connect(ui->listWidget, SIGNAL(clicked(const QModelIndex &)), this, SLOT(singleclicked(const QModelIndex &)));
	connect(ui->horizontalFrame, SIGNAL(aboutTohide()), this, SLOT(onAboutToHide()));
	connect(ui->horizontalFrame, SIGNAL(waitToHide()), this, SLOT(onWaitToHide()));
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onHideFrame()));
	ui->hideLabel->hide();
	//ui->verticalFrame->hideWidget();

	/*connect(ui->axWidget, SIGNAL(OnLoginComplete()), this, SLOT(onLoginComplete()));
	connect(ui->axWidget, SIGNAL(OnDisconnected(int)), this, SLOT(onDisconnected(int)));
	connect(ui->axWidget, SIGNAL(OnAuthenticationWarningDismissed()), this, SLOT(onAuthenticationWarningDismissed()));
	connect(ui->axWidget, SIGNAL(OnAuthenticationWarningDisplayed()), this, SLOT(onAuthenticationWarningDisplayed()));
	connect(ui->axWidget, SIGNAL(OnLogonError(int)), this, SLOT(onLogonError(int)));
	connect(ui->axWidget, SIGNAL(OnNetworkStatusChanged(uint, int, int)), this, SLOT(onNetworkStatusChanged(uint, int, int)));

	connect(ui->axWidget, SIGNAL(OnLoginComplete()), webSocket, SLOT(onLoginComplete()));*/
	connect(&axWidgetManage, SIGNAL(loginComplete(QString &)), webSocket, SLOT(onLoginComplete()));
	connect(&axWidgetManage, SIGNAL(loginComplete(QString &)), this, SLOT(onLoginComplete(QString &)));
	connect(&axWidgetManage, SIGNAL(disconnected(QString &)), this, SLOT(onDisconnected(QString &)));
	connect(&axWidgetManage, SIGNAL(toConnect(QString &)), this, SLOT(onConnectAxWidget(QString &)));

	connect(webSocket, SIGNAL(showUrl(QString&)), ui->loginWidget, SLOT(onShowUrl(QString&)));
	connect(webSocket, SIGNAL(scan()), ui->loginWidget, SLOT(onShowMovie()));
#ifndef ONE_CLOUD_USE
	connect(webSocket, SIGNAL(textReceived(MstscRdpInfo &, QString &)), this, SLOT(onTextReceived(MstscRdpInfo &, QString &)));
#endif // !ONE_CLOUD_USE
    connect(webSocket->m_loginDataHandler, SIGNAL(textReceived(MstscRdpInfo &, QString &)), this, SLOT(onTextReceived(MstscRdpInfo &, QString &)));
	connect(webSocket, SIGNAL(login()), this, SLOT(onLogin()));
	connect(webSocket, SIGNAL(disconnected()), ui->loginWidget, SLOT(onDisconnected()));
	connect(webSocket, SIGNAL(userInfo(QString &, QString &, QString &)), this, SLOT(onUserInfo(QString &, QString &, QString &)));
	connect(webSocket, SIGNAL(raiseError(int,QString &)), this, SLOT(onWebSocketRaiseError(int,QString&)));
    connect(secondScanWebSocket, SIGNAL(textReceived(MstscRdpInfo &, QString &)), this, SLOT(onTextReceived(MstscRdpInfo &, QString &)));

	connect(ui->loginWidget, SIGNAL(clicked()), webSocket, SLOT(updataUrl()));
	connect(ui->loginWidget, SIGNAL(updateBtnClicked()), webSocket, SLOT(reconnect()));
    connect(ui->loginWidget, SIGNAL(h5chatLogin(QString &, ChatServerInfo &)), this, SLOT(onH5chatLogin(QString &, ChatServerInfo &)));

	//VncSshDesktop *pbVncSshDesktop = new VncSshDesktop();
	//connect(webSocket, SIGNAL(textReceived(QString &)), pbVncSshDesktop, SLOT(getLoginUrl(QString &)));

    connect(pbVncSshDesktop, SIGNAL(networkComplete(QString &, QString &)), this, SLOT(onNetworkComplete(QString &, QString &)));
    connect(cloudblkmgr->m_vncSshDesktop, SIGNAL(networkComplete(QString &, QString &)), this, SLOT(onNetworkComplete(QString &, QString &)));
	connect(cloudblkmgr, SIGNAL(textReceived(MstscRdpInfo &, QString &)), this, SLOT(onTextReceived(MstscRdpInfo &, QString &)));
	connect(cloudblkmgr, SIGNAL(textReceived(QList<MstscRdpInfo>&)), this, SLOT(onTextReceived(QList<MstscRdpInfo>&)));
    connect(webSocket, SIGNAL(localLogin(MstscRdpInfo &)), this, SLOT(onLocalLogin(MstscRdpInfo &)));

	webSocket->start();
    
	//isConnect = false;

	connect(ui->loginWidget, SIGNAL(localLogin(MstscRdpInfo &)), this, SLOT(onLocalLogin(MstscRdpInfo &)));
	connect(ui->loginWidget, SIGNAL(close()), this, SLOT(onClose()));
	connect(ui->loginWidget, SIGNAL(toMin()), this, SLOT(showMinimized()));

	connect(ui->setBtn, SIGNAL(clicked()), this, SLOT(on_setBtn_clicked()));

	connect(ui->toolBtn, SIGNAL(clicked()), this, SLOT(onToolsBtnClicked()));

	connect(&axWidgetManage, SIGNAL(toClose(QString&)), this, SLOT(OnClose(QString&)));

	MstscRdpInfo mstscRdpInfo;
	SingleTon<Settings>::getInstance().beginGroup("RemoteDesktop");
	mstscRdpInfo.fulladdress = SingleTon<Settings>::getInstance().value("ip").toString();
	mstscRdpInfo.port = SingleTon<Settings>::getInstance().value("port", 0).toInt();
	mstscRdpInfo.username = SingleTon<Settings>::getInstance().value("username").toString();

	if (ui->loginWidget->remember()) {
		if (SingleTon<Settings>::getInstance().value("password").toString().length() > 0) {
			QByteArray ary = SingleTon<Settings>::getInstance().value("password").toString().toLatin1();
			xorData(ary);
			mstscRdpInfo.password = QString(ary);
		}
	}
	mstscRdpInfo.domain = SingleTon<Settings>::getInstance().value("domain").toString();
	SingleTon<Settings>::getInstance().endGroup();

	ui->loginWidget->onLoginInfo(mstscRdpInfo);

	//测试
	mstscRdpInfo.fulladdress = "192.168.1.112";//"172.28.93.189";//"192.168.1.112";//"183.62.15.115";
	mstscRdpInfo.port = 0;// 11003;
	mstscRdpInfo.username = "admin";// "QApublic3";
	mstscRdpInfo.password = "engine";
	//mstscRdpInfo.domain = "od.com";

	//onLogin();
	//onTextReceived(mstscRdpInfo);

	//axWidgetManage.openAxWidget(mstscRdpInfo);
	//ui->widget_4->show();

#ifndef ONE_CLOUD_USE
	m_pRdpsDockWidget = new DockWidget(ui->widget_3);

	m_pListWidget = new QListWidget();
	m_pListWidget->setContentsMargins(0, 0, 0, 0);
	m_pListWidget->setAutoScrollMargin(0);
	m_pListWidget->setResizeMode(QListView::ResizeMode::Adjust);
	m_pListWidget->setAutoScroll(true);
	m_pListWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidget->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidget->setGridSize(QSize(222, 172));

	m_pRdpsDockWidget->SetThumbListWidget(m_pListWidget);

	m_dockMaps.insert(ui->listWidget->item(3)->whatsThis(), m_pRdpsDockWidget);

	m_pSSHDockWidget = new DockWidget(ui->widget_3);

	m_pListWidgetSSH = new QListWidget();
	m_pListWidgetSSH->setContentsMargins(0, 0, 0, 0);
	m_pListWidgetSSH->setAutoScrollMargin(0);
	m_pListWidgetSSH->setResizeMode(QListView::ResizeMode::Adjust);
	m_pListWidgetSSH->setAutoScroll(true);
	m_pListWidgetSSH->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidgetSSH->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidgetSSH->setGridSize(QSize(222, 172));

	m_pSSHDockWidget->SetThumbListWidget(m_pListWidgetSSH);

	m_dockMaps.insert(ui->listWidget->item(6)->whatsThis(), m_pSSHDockWidget);

	m_pVNCDockWidget = new DockWidget(ui->widget_3);

	m_pListWidgetVNC = new QListWidget();
	m_pListWidgetVNC->setContentsMargins(0, 0, 0, 0);
	m_pListWidgetVNC->setAutoScrollMargin(0);
	m_pListWidgetVNC->setResizeMode(QListView::ResizeMode::Adjust);
	m_pListWidgetVNC->setAutoScroll(true);
	m_pListWidgetVNC->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidgetVNC->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidgetVNC->setGridSize(QSize(222, 172));
	m_pVNCDockWidget->SetThumbListWidget(m_pListWidgetVNC);

	m_dockMaps.insert(ui->listWidget->item(5)->whatsThis(), m_pVNCDockWidget);

	m_pShareDockWidget = new DockWidget(ui->widget_3);

	m_pListWidgetShare = new QListWidget();
	m_pListWidgetShare->setContentsMargins(0, 0, 0, 0);
	m_pListWidgetShare->setAutoScrollMargin(0);
	m_pListWidgetShare->setResizeMode(QListView::ResizeMode::Adjust);
	m_pListWidgetShare->setAutoScroll(true);
	m_pListWidgetShare->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidgetShare->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidgetShare->setGridSize(QSize(222, 172));
	m_pShareDockWidget->SetThumbListWidget(m_pListWidgetShare);

	m_dockMaps.insert(ui->listWidget->item(4)->whatsThis(), m_pShareDockWidget);

	m_pWebsiteDockWidget = new DockWidget(ui->widget_3);

	m_pListWidgetWebsite = new QListWidget();
	m_pListWidgetWebsite->setContentsMargins(0, 0, 0, 0);
	m_pListWidgetWebsite->setAutoScrollMargin(0);
	m_pListWidgetWebsite->setResizeMode(QListView::ResizeMode::Adjust);
	m_pListWidgetWebsite->setAutoScroll(true);
	m_pListWidgetWebsite->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidgetWebsite->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
	m_pListWidgetWebsite->setGridSize(QSize(222, 172));
	m_pWebsiteDockWidget->SetThumbListWidget(m_pListWidgetWebsite);
	m_dockMaps.insert(ui->listWidget->item(8)->whatsThis(), m_pWebsiteDockWidget);

#endif
	m_pDockMenu = new DockMenu(ui->widget_3);
	m_pDockMenu->setGeometry(100, this->geometry().height() - m_pDockMenu->size().height(), m_pDockMenu->size().width(), m_pDockMenu->size().height());

	connect(m_pDockMenu->m_pBtnMin, SIGNAL(clicked()), this, SLOT(showMinimized()));
	connect(m_pDockMenu->m_pBtnExit, SIGNAL(clicked()), this, SLOT(on_exitBtn_clicked()));
	connect(m_pDockMenu->m_pBtnLock, SIGNAL(clicked()), this, SLOT(on_lockBtn_clicked()));


	ui->horizontalFrame->setDockMenu(m_pDockMenu);

	m_pToolMenu = new ToolMenu(ui->widget_3);
	m_pToolMenu->setGeometry(100, this->geometry().height() - 120 + (60 - 71) / 2, 100, 71);
	ui->horizontalFrame->setToolMenu(m_pToolMenu);
	connect(m_pToolMenu->m_pBtnScreenShot, SIGNAL(clicked()), this, SLOT(onScreenShotClicked()));

	connect(m_pToolMenu->m_pBtnScreenRec, SIGNAL(clicked()), this, SLOT(onScreenRecClicked()));

	connect(ui->upBtn, SIGNAL(clicked()), this, SLOT(on_upBtn_clicked()));
	connect(ui->downBtn, SIGNAL(clicked()), this, SLOT(on_downBtn_clicked()));

	//m_pSreenRecFrame = new ScreenRecFrame(this);
	//m_pSreenRecFrame->hide();
	//RegisterHotKey((HWND)PowerdudeMain::winId(),   // Set the system identifier of the widget window that will handle the HotKey
	//	100,                         // Set identifier HotKey
	//	MOD_ALT,         // Set modifiers
	//	VK_TAB);
}


PowerdudeMain::~PowerdudeMain()
{

}

void PowerdudeMain::onWebSocketRaiseError(int iErrorType, QString& errorMsg)
{
	if (-1 == iErrorType) {
		ui->loginWidgetFrame->hide();
		OneCloudMessageBox::information(this, errorMsg);
	}
	else if (-2 == iErrorType) {
		ui->loginWidgetFrame->hide();
		OneCloudMessageBox::information(this, errorMsg);
	}
}

//bool PowerdudeMain::nativeEvent(const QByteArray &eventType, void *message, long *result)
//{
//	Q_UNUSED(eventType)
//		Q_UNUSED(result)
//		// Transform the message pointer to the MSG WinAPI
//		MSG* msg = reinterpret_cast<MSG*>(message);
//
//	// If the message is a HotKey, then ...
//	if (msg->message == WM_HOTKEY) {
//		// ... check HotKey
//		if (msg->wParam == 100) {
//			// We inform about this to the console
//			qDebug() << "HotKey worked";
//			onAltAndTab();
//			return true;
//		}
//	}
//	return false;
//}

void PowerdudeMain::onScreenShotClicked()
{
	m_pToolMenu->hide();
	ui->horizontalFrame->hideWidget(false);
	QTimer::singleShot(500, this, SLOT(onTimerScreenShot()));
}

void PowerdudeMain::onAltAndG()
{
	onScreenRecClicked();
}

void PowerdudeMain::onScreenRecClicked()
{
	if (m_pScreenRecordDialog == 0) {
		int iResult = 0;
		m_pScreenRecordDialog = new ScreenRecordDialog(webSocket, iResult, this);
		if (QDialog::Accepted == m_pScreenRecordDialog->exec()) {
			if (iResult == -1) {
				OneCloudMessageBox::information(this, QStringLiteral("录屏正在进行中..."));
			}
			else if (iResult == -2) {
				OneCloudMessageBox::information(this, QStringLiteral("启动录屏程序失败..."));
			}
		}
		delete m_pScreenRecordDialog;
		m_pScreenRecordDialog = 0;
	}
}

void PowerdudeMain::onTimerScreenShot()
{
	QSimpleScreenShot::instance()->screenShot(QApplication::activeModalWidget());
}

void PowerdudeMain::onCtrlAndA()
{
	QTimer::singleShot(1, this, SLOT(onTimerScreenShot()));
}

void PowerdudeMain::startHookRDPDisplay()
{
	HMODULE hModule = ::LoadLibrary(L"Gdi32.dll");
	if (0 != hModule) {
		g_pfBitBlt = (pfBitBlt)::GetProcAddress(hModule, "BitBlt");
		Mhook_SetHook((PVOID*)&g_pfBitBlt, reinterpret_cast<PVOID>(BitBltProc));
	}
}

ThumbWidget* PowerdudeMain::getThumbWidget(HWND hWnd)
{
	ThumbWidget* pThumb = 0;
#ifndef ONE_CLOUD_USE
	for (int i = 0; i < m_pListWidget->count(); i++) {
		ThumbWidget* thumb = (ThumbWidget*)m_pListWidget->itemWidget(m_pListWidget->item(i));
		if (0 != thumb && thumb->GetHwnd() == hWnd) {
			pThumb = thumb;
			break;
		}
	}
#endif
	return pThumb;
}



void PowerdudeMain::highLightUpdate()
{
	for (int i = 0;i < ui->listWidget->count();i++) {
		if (ui->listWidget->item(i)->data(100).toInt()<100) {
			if (axWidgetManage.contains(ui->listWidget->item(i)->whatsThis())) {
				ui->listWidget->item(i)->setSelected(true);
			}
			else {
				ui->listWidget->item(i)->setSelected(false);
			}
		}
		else {
			InfoListWidget* pInfoListWidget = infoListWidgetList[ui->listWidget->item(i)->data(100).toInt()-100];
			if (pInfoListWidget->isXBtnShow()) {
				ui->listWidget->item(i)->setSelected(true);
			}
			else {
				ui->listWidget->item(i)->setSelected(false);
			}
		}
	}
}

QRect PowerdudeMain::checkRect(int ax, int ay, int aw, int ah)
{
	if (ax < 0)ax = 0;
	if (ay < 0)ay = 0;
	if (ax + aw > this->rect().width()) {
		ax -= ax + aw - this->rect().width();
	}
	if (ay + ah > this->rect().height()) {
		ay -= ay + ah - this->rect().height();
	}
	return QRect(ax, ay, aw, ah);
}

void PowerdudeMain::singleclicked(QListWidgetItem* item)
{
	/*if (ui->listWidget->currentRow() == 3 && item->isSelected()) {
		QList<MstscRdpInfo> list = cloudblkmgr->getDesktopList(webSocket->m_chatSvrInfo, "/business/user/desktop/list");
		onTextReceived(list);
	}*/

	/*if (ui->listWidget->currentRow() == 5 && item->isSelected()) {
		QList<MstscRdpInfo> list = cloudblkmgr->getVncDesktopList(webSocket->m_chatSvrInfo, "/business/user/vnc/list");
		onTextReceived(list);
	}*/

	DockWidget* pDockWidget = 0;
	if (m_dockMaps.contains(item->whatsThis())) {
		pDockWidget = m_dockMaps.find(item->whatsThis()).value();
	}
	if (pDockWidget) {
		ui->horizontalFrame->unRegisterDocketWidget();
	}

	if (item->data(100).toInt()>=100){//ui->listWidget->currentRow() >= 3 && ui->listWidget->currentRow() < 3 + infoListWidgetList.size()-1 && item == ui->listWidget->item(ui->listWidget->currentRow())/*item->isSelected()*/) {
		InfoListWidget* pInfoListWidget = infoListWidgetList[item->data(100).toInt() - 100];

		if (ui->listWidget->currentRow() == 3 || webSocket->isLogin()) {
			/*if (pInfoListWidget->isXBtnShow()) {
				item->setSelected(true);
			}*/
			if (pInfoListWidget->isHidden()) {
				pInfoListWidget->resizeFrame();

				ui->horizontalFrame->setGeometry(0, 0, 520, this->size().height());
				ui->horizontalFrame->setAutoHideSize(QSize(520, this->size().height()));
				//ui->sysFrame->hide();
				for (int i = 0;i < infoListWidgetList.size();i++) {
					infoListWidgetList[i]->hide();
				}
				pInfoListWidget->show();
			}
			else {
				pInfoListWidget->hide();
				ui->horizontalFrame->setGeometry(0, 0, ui->verticalFrame->width(), this->size().height());
				ui->horizontalFrame->setAutoHideSize(QSize(ui->verticalFrame->width(), this->size().height()));
			}
		}
		else {
			on_openLoginBtn_clicked();
		}
	}
	else if (/*item->isSelected()*/item == ui->listWidget->item(ui->listWidget->currentRow())) {
		if (webSocket->isLogin()) {
			//highLightUpdate();
			if (ui->listWidget->currentRow() == 0) {
				if (axWidgetManage.contains(item->whatsThis())) {
					ThumbWidget* thumb = axWidgetManage.openWebEngineView(item->whatsThis(), item->whatsThis(), item->icon());
					if (thumb != 0) {
						DockWidget* pDockWidget = item->data(10000).value<DockWidget*>();
						if (pDockWidget == 0) {
							pDockWidget = new DockWidget(ui->widget_3);
							item->setData(10000, QVariant::fromValue(pDockWidget));
							connect(thumb, &ThumbWidget::thumbCloseWidget, [&](QString& key, bool isAxWidget) {
								axWidgetManage.closeWebEngineView(key);
							});

							connect(thumb, &ThumbWidget::thumbOpenWidget, [&](QString& key, bool isAxWidget) {
								axWidgetManage.openWebEngineView(key);
							});
						}
						pDockWidget->SetThumbWidget(thumb);
						item->setData(10000, QVariant::fromValue(thumb));
					}
				}
				else {
					cloudblkmgr->getH5ChatLoginUrl(webSocket->m_chatSvrInfo, "/business/user/chat/login");
				}
			}
			else if (ui->listWidget->currentRow() == 2) {
				if (axWidgetManage.contains(item->whatsThis())) {
					ThumbWidget* thumb = axWidgetManage.openWebEngineView(item->whatsThis(), item->whatsThis(), item->icon());
					if (thumb != 0) {
						item->setData(10000, QVariant::fromValue(thumb));
					}
				}
				else {
					cloudblkmgr->getSpaceLoginUrl(webSocket->m_chatSvrInfo, "/business/user/space/file/login");
				}
			}
			else {
				ThumbWidget* thumb = axWidgetManage.openWebEngineView(item->whatsThis(), item->whatsThis(), item->icon());
				if (thumb != 0) {
					DockWidget* pDockWidget = 0;
					if (m_dockMaps.contains(item->whatsThis())) {
						pDockWidget = m_dockMaps.find(item->whatsThis()).value();
					}
					if (pDockWidget == 0) {
						pDockWidget = new DockWidget(ui->widget_3);
						connect(thumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onCloseWebEngineViewClicked(QString&, bool)));
						connect(thumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onOpenWebEngineViewClicked(QString&, bool)));
						if (!m_dockMaps.contains(item->whatsThis())) {
							m_dockMaps.insert(item->whatsThis(), pDockWidget);
						}
					}
					pDockWidget->SetThumbWidget(thumb);
				}
			}
		}
		else {
			on_openLoginBtn_clicked();
		}
	}
	highLightUpdate();
}

void PowerdudeMain::onCloseWebEngineViewClicked(QString& key, bool isAxWidget)
{
	ui->horizontalFrame->unRegisterDocketWidget();
	axWidgetManage.closeWebEngineView(key);
	DockWidget* pDockWidget = m_dockMaps.find(key).value();
	if (pDockWidget != 0) {
		disconnect(pDockWidget->GetThumbWidget(), SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onCloseWebEngineViewClicked(QString& key, bool isAxWidget)));
		disconnect(pDockWidget->GetThumbWidget(), SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onOpenWebEngineViewClicked(QString& key, bool isAxWidget)));
		m_dockMaps.remove(key);
		pDockWidget->deleteLater();
	}
}

void PowerdudeMain::onOpenWebEngineViewClicked(QString& key, bool isAxWidget)
{
	axWidgetManage.openWebEngineView(key);
}

void PowerdudeMain::onListCloseWebEngineViewClicked(QString& key, bool isAxWidget)
{
	InfoWidget* pInfoWidget = infoListWidgetList[3]->getInfoWidget(key);
	if (pInfoWidget != 0) {
		ThumbWidget* pThumb = 0;
		if (m_sshThumbsMap.contains(key)) {
			pThumb = m_sshThumbsMap.find(key).value();
			m_sshThumbsMap.remove(key);
			disconnect(pThumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
			disconnect(pThumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
			int iRow = m_pListWidgetSSH->row(pThumb->GetItem());
			m_pListWidgetSSH->removeItemWidget(pThumb->GetItem());
			pThumb->SetItem(NULL);
			m_pListWidgetSSH->takeItem(iRow);
			int iHeight = 174 * m_pListWidgetSSH->count();
			m_pListWidgetSSH->resize(222, iHeight);
			m_pListWidgetSSH->update();
			QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(6));
			int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
			m_pListWidgetSSH->resize(222, iHeight);
			m_pSSHDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			pThumb->deleteLater();
		}
		emit pInfoWidget->toClose();
	}
	pInfoWidget = infoListWidgetList[2]->getInfoWidget(key);
	if (pInfoWidget != 0) {
		ThumbWidget* pThumb = 0;
		if (m_vncThumbsMap.contains(key)) {
			pThumb = m_vncThumbsMap.find(key).value();
			m_vncThumbsMap.remove(key);
			disconnect(pThumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
			disconnect(pThumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
			int iRow = m_pListWidgetVNC->row(pThumb->GetItem());
			m_pListWidgetVNC->removeItemWidget(pThumb->GetItem());
			pThumb->SetItem(NULL);
			m_pListWidgetVNC->takeItem(iRow);
			int iHeight = 174 * m_pListWidgetVNC->count();
			m_pListWidgetVNC->resize(222, iHeight);
			m_pListWidgetVNC->update();
			QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(5));
			int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
			m_pListWidgetVNC->resize(222, iHeight);
			m_pVNCDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			pThumb->deleteLater();
		}
		emit pInfoWidget->toClose();
	}
	pInfoWidget = infoListWidgetList[1]->getInfoWidget(key);
	if (pInfoWidget != 0) {
		ThumbWidget* pThumb = 0;
		if (m_shareThumbsMap.contains(key)) {
			pThumb = m_shareThumbsMap.find(key).value();
			m_shareThumbsMap.remove(key);
			disconnect(pThumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
			disconnect(pThumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
			int iRow = m_pListWidgetShare->row(pThumb->GetItem());
			m_pListWidgetShare->removeItemWidget(pThumb->GetItem());
			pThumb->SetItem(NULL);
			m_pListWidgetShare->takeItem(iRow);
			int iHeight = 174 * m_pListWidgetShare->count();
			m_pListWidgetShare->resize(222, iHeight);
			m_pListWidgetShare->update();
			QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(4));
			int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
			m_pListWidgetShare->resize(222, iHeight);
			m_pShareDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			pThumb->deleteLater();
		}
		emit pInfoWidget->toClose();
	}
	pInfoWidget = infoListWidgetList[4]->getInfoWidget(key);
	if (pInfoWidget != 0) {
		ThumbWidget* pThumb = 0;
		if (m_websiteThumbsMap.contains(key)) {
			pThumb = m_websiteThumbsMap.find(key).value();
			m_websiteThumbsMap.remove(key);
			disconnect(pThumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
			disconnect(pThumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
			int iRow = m_pListWidgetWebsite->row(pThumb->GetItem());
			m_pListWidgetWebsite->removeItemWidget(pThumb->GetItem());
			pThumb->SetItem(NULL);
			m_pListWidgetWebsite->takeItem(iRow);
			int iHeight = 174 * m_pListWidgetWebsite->count();
			m_pListWidgetWebsite->resize(222, iHeight);
			m_pListWidgetWebsite->update();
			QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(8));
			int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
			m_pListWidgetWebsite->resize(222, iHeight);
			m_pWebsiteDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			pThumb->deleteLater();
		}
		emit pInfoWidget->toClose();
	}
}

void PowerdudeMain::onListOpenWebEngineViewClicked(QString& key, bool isAxWidget)
{
	axWidgetManage.openWebEngineView(key);
}


void PowerdudeMain::singleclicked2(QListWidgetItem * item)
{
	int type = item->data(Type).toInt();
	item->setSelected(item->data(Selected).toBool());
	
	if (controlPanelInfoMap.contains(type)) {
		controlPanelInfo& cpInfo = controlPanelInfoMap[type];
		InfoListWidget* pInfoListWidget = cpInfo.pInfoListWidget;
		if (pInfoListWidget != NULL) {
			if (type == 2 || webSocket->isLogin()) {
				if (pInfoListWidget->isHidden()) {
					pInfoListWidget->resizeFrame();

					ui->horizontalFrame->setGeometry(0, 0, 520, this->size().height());
					ui->horizontalFrame->setAutoHideSize(QSize(520, this->size().height()));

					for (auto iter = controlPanelInfoMap.begin(); iter != controlPanelInfoMap.end(); ++iter)
					{
						if (iter.value().pInfoListWidget != NULL) {
							iter.value().pInfoListWidget->hide();
						}
					}
					pInfoListWidget->show();
				}
				else {
					pInfoListWidget->hide();
					ui->horizontalFrame->setGeometry(0, 0, ui->verticalFrame->width(), this->size().height());
					ui->horizontalFrame->setAutoHideSize(QSize(ui->verticalFrame->width(), this->size().height()));
				}
			}
			else {
				on_openLoginBtn_clicked();
			}
		}
		else if (item == ui->listWidget->item(ui->listWidget->currentRow())) {
			if (webSocket->isLogin()) {
				if (oneCloudManage.contains(cpInfo.key))
				{
					oneCloudManage.openOneCloudMainWindow(cpInfo.key);
				}
				else {
					OneCloudData* pData = oneCloudDataMap[cpInfo.key];
					if (pData != NULL) {
						OneCloudView* pView = OneCloudDataFactory::createOneCloudView(type, pData);
						oneCloudManage.openOneCloudMainWindow(cpInfo.key, pData->getData("name").toString(), pView, cpInfo.icon);
					}
				}
			}
			else {
				on_openLoginBtn_clicked();
			}
		}
	}
}

void PowerdudeMain::onValueChanged(int value)
{
	if (ui->listWidget->verticalScrollBar()->value() == 0) {
		ui->upBtn->setEnabled(false);
	}
	else {
		ui->upBtn->setEnabled(true);
	}

	if (ui->listWidget->verticalScrollBar()->value() == ui->listWidget->verticalScrollBar()->maximum()) {
		ui->downBtn->setEnabled(false);
	}
	else {
		ui->downBtn->setEnabled(true);
	}
}

void PowerdudeMain::onAboutToHide()
{
#ifndef ONE_CLOUD_USE
	if (!m_pDockMenu->isHidden()) {
		m_pDockMenu->hide();
	}
	if (!m_pToolMenu->isHidden()) {
		m_pToolMenu->hide();
	}
#endif
	if (ui->horizontalFrame == sender()) {
#ifndef ONE_CLOUD_USE
		for (int i = 0;i < infoListWidgetList.size();i++) {
			infoListWidgetList[i]->hide();
		}
#else
		for (auto iter = controlPanelInfoMap.begin(); iter != controlPanelInfoMap.end(); ++iter)
		{
			if (iter.value().pInfoListWidget != NULL) {
				iter.value().pInfoListWidget->hide();
			}
		}
#endif

		ui->horizontalFrame->setGeometry(0, 0, 100, this->size().height());
		ui->horizontalFrame->setAutoHideSize(QSize(100, this->size().height()));
	}
}

void PowerdudeMain::onWaitToHide()
{
	SingleTon<Settings>::getInstance().beginGroup("Globa");
	bool en = SingleTon<Settings>::getInstance().value("hotKeyEn", true).toBool();
	bool altA = SingleTon<Settings>::getInstance().value("altA", true).toBool();
	bool altAHide = SingleTon<Settings>::getInstance().value("altAHide", false).toBool();
	SingleTon<Settings>::getInstance().endGroup();
	if (en&&altA) {
		if(!altAHide)ui->hideLabel->show();
		m_pTimer->start(3000);
		waitHide = true;
	}
	else {
		ui->horizontalFrame->ToHideWidget();
	}
	
}

void PowerdudeMain::onHideFrame()
{
	ui->hideLabel->hide();
	waitHide = false;
}

void PowerdudeMain::connectRemoteSystem()
{
	//ui->axWidget->dynamicCall("Connect()");
	//mRemoteSystemView->dynamicCall("RedirectNow(int)",0);
}

void PowerdudeMain::disconnectRemoteSystem()
{
	//if (ui->axWidget != NULL)
	//	ui->axWidget->dynamicCall("Disconnect()");
}

void PowerdudeMain::closeEvent(QCloseEvent * event)
{
	disconnectRemoteSystem();
	emit aboutToClose();
	this->close();
}

void PowerdudeMain::changeEvent(QEvent *e)
{
	switch (e->type()) {

	case QEvent::WindowStateChange:
		if (!isMinimized()) {
			setAttribute(Qt::WA_Mapped);
		}

		/*if (this->windowState() == Qt::WindowMinimized)
		{
			SingleTon<Settings>::getInstance().beginGroup("Globa");
			bool en = SingleTon<Settings>::getInstance().value("hotKeyEn", false).toBool();
			SingleTon<Settings>::getInstance().endGroup();

			if (en) {
				SingleTon<Hook>::getInstance().hook(false);
			}
		}else if (this->windowState() == Qt::WindowMaximized)
		{
			SingleTon<Settings>::getInstance().beginGroup("Globa");
			bool en = SingleTon<Settings>::getInstance().value("hotKeyEn", false).toBool();
			SingleTon<Settings>::getInstance().endGroup();

			if (en) {
				SingleTon<Hook>::getInstance().hook(true);
			}
		}*/
		break;

	default:
		break;
	}
	QWidget::changeEvent(e);
}

void PowerdudeMain::onToolsBtnClicked()
{
	m_pDockMenu->hide();
	if (m_pToolMenu->isHidden()) {
		ui->horizontalFrame->setGeometry(0, 0, ui->verticalFrame->width(), this->size().height());
		ui->horizontalFrame->setAutoHideSize(QSize(ui->verticalFrame->width(), this->size().height()));
		for (int i = 0; i < infoListWidgetList.size(); i++) {
			infoListWidgetList[i]->hide();
		}
		ui->horizontalFrame->update();
		m_pToolMenu->update();
		ui->widget_3->update();
		m_pToolMenu->show();
	}
	else {
		m_pToolMenu->update();
		m_pToolMenu->hide();
	}
}

void PowerdudeMain::on_sysBtn_clicked()
{
//#ifndef ONE_CLOUD_USE
	m_pToolMenu->hide();
	if (m_pDockMenu->isHidden()) {
		ui->horizontalFrame->setGeometry(0, 0, ui->verticalFrame->width(), this->size().height());
		ui->horizontalFrame->setAutoHideSize(QSize(ui->verticalFrame->width(), this->size().height()));
#ifndef ONE_CLOUD_USE
		for (int i = 0; i < infoListWidgetList.size(); i++) {
			infoListWidgetList[i]->hide();
		}
#else
		for (auto iter = controlPanelInfoMap.begin(); iter != controlPanelInfoMap.end(); ++iter)
		{
			if (iter.value().pInfoListWidget != NULL) {
				iter.value().pInfoListWidget->hide();
			}
		}
#endif
		ui->horizontalFrame->update();
		m_pDockMenu->update();
		ui->widget_3->update();
		m_pDockMenu->show();
		//m_pDockMenu->m_pBtnLock->setEnabled(webSocket->isLogin());
		if (webSocket->isLogin()) {
			m_pDockMenu->m_pBtnLock->show();
		}
		else {
			m_pDockMenu->m_pBtnLock->hide();
		}
	}
	else {
		m_pDockMenu->update();
		m_pDockMenu->hide();
	}
//#endif // !ONE_CLOUD_USE

	
	/*if (ui->sysFrame->isHidden()) {
		ui->horizontalFrame->setGeometry(0, 0, ui->verticalFrame->width(), this->size().height());
		ui->horizontalFrame->setAutoHideSize(QSize(ui->verticalFrame->width(), this->size().height()));

		for (int i = 0;i < infoListWidgetList.size();i++) {
			infoListWidgetList[i]->hide();
		}
		ui->sysFrame->show();
	}
	else {
		ui->sysFrame->hide();
		ui->horizontalFrame->setGeometry(0, 0, ui->verticalFrame->width(), this->size().height());
		ui->horizontalFrame->setAutoHideSize(QSize(ui->verticalFrame->width(), this->size().height()));

	}*/
}

void PowerdudeMain::on_setBtn_clicked()
{
	if (m_pSetDialog == 0) {
		m_pSetDialog = new SetDialog(this);
		m_pSetDialog->exec();
		delete m_pSetDialog;
		m_pSetDialog = 0;
	}
	

	//if (ret == QDialog::Accepted)
	/*{
		SingleTon<Settings>::getInstance().beginGroup("Globa");
		bool en = SingleTon<Settings>::getInstance().value("hotKeyEn", true).toBool();
		SingleTon<Settings>::getInstance().endGroup();

		if (en) {
			SingleTon<Hook>::getInstance().hook(true);
		}
		else {
			SingleTon<Hook>::getInstance().hook(false);
		}
	}*/
	//delete dlgSet;
}

void PowerdudeMain::onItemEntered(QListWidgetItem * item)
{
#ifndef ONE_CLOUD_USE
	ui->horizontalFrame->unRegisterDocketWidget();
	DockWidget* pDockWidget = 0;
	if (m_dockMaps.contains(item->whatsThis())) {
		pDockWidget = m_dockMaps.find(item->whatsThis()).value();
	}
	QRect position = ui->listWidget->visualItemRect(item);
	if (item == ui->listWidget->item(3)) {
		if (m_pRdpsDockWidget != 0) {
			int iHeight = 174 * m_pListWidget->count();
			int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
			m_pListWidget->resize(222, iHeight);
			m_pRdpsDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			ui->horizontalFrame->registerDockWidget(m_pRdpsDockWidget);
		}
	}
	else if (item == ui->listWidget->item(4)) {//tongping
		if (m_pListWidgetShare != 0) {
			int iHeight = 174 * m_pListWidgetShare->count();
			int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
			m_pListWidgetShare->resize(222, iHeight);
			m_pShareDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			ui->horizontalFrame->registerDockWidget(m_pShareDockWidget);
		}
	}
	else if (item == ui->listWidget->item(5)) {//vnc
		if (m_pListWidgetVNC != 0) {
			int iHeight = 174 * m_pListWidgetVNC->count();
			int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
			m_pListWidgetVNC->resize(222, iHeight);
			m_pVNCDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			ui->horizontalFrame->registerDockWidget(m_pVNCDockWidget);
		}
	}
	else if (item == ui->listWidget->item(6)) {//ssh
		if (m_pListWidgetSSH != 0) {
			int iHeight = 174 * m_pListWidgetSSH->count();
			int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
			m_pListWidgetSSH->resize(222, iHeight);
			m_pSSHDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			ui->horizontalFrame->registerDockWidget(m_pSSHDockWidget);
		}
	}
	else if (item == ui->listWidget->item(8)) {//website
		if (m_pListWidgetWebsite != 0) {
			int iHeight = 174 * m_pListWidgetWebsite->count();
			int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
			m_pListWidgetWebsite->resize(222, iHeight);
			m_pWebsiteDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			ui->horizontalFrame->registerDockWidget(m_pWebsiteDockWidget);
		}
	}
	else {
		if (pDockWidget != 0) {
			pDockWidget->setGeometry(100, qMax<int>(0, position.y()), 222, 172);
			ui->horizontalFrame->registerDockWidget(pDockWidget);
		}
	}
	if (!m_pDockMenu->isHidden()) {
		m_pDockMenu->hide();
	}
	if (!m_pToolMenu->isHidden()) {
		m_pToolMenu->hide();
	}
#endif
}

void PowerdudeMain::on_lockBtn_clicked()
{
	if (webSocket->isLogin()) {
		Lock * pLock = new Lock();
		pLock->setUserInfo(userInfo);
		pLock->setChatServerApi(webSocket->m_chatSvrInfo.chatServerApi, webSocket->m_chatSvrInfo.token);
		pLock->show();
		pLock->activateWindow();

		this->hide();

		connect(pLock, SIGNAL(unLock()), this, SLOT(onUnLock()));
		connect(pLock, SIGNAL(minClicked()), this, SLOT(showMinimized()));
		connect(pLock, SIGNAL(exitClicked()), this, SLOT(on_lockExitBtn_clicked()));
	}
}

void PowerdudeMain::on_upBtn_clicked()
{
	ui->listWidget->verticalScrollBar()->setValue(ui->listWidget->verticalScrollBar()->value() - ui->listWidget->gridSize().height());
}

void PowerdudeMain::on_lockExitBtn_clicked()
{
	onUnLock();
	on_exitBtn_clicked();
}

void PowerdudeMain::on_exitBtn_clicked()
{
#ifndef ONE_CLOUD_USE
	ui->horizontalFrame->unRegisterDocketWidget();
	QMap<QString, DockWidget*>::const_iterator i;
	for (i = m_dockMaps.constBegin(); i != m_dockMaps.constEnd(); ++i) {
		DockWidget *pDockMap = i.value();
		if (pDockMap != m_pRdpsDockWidget && pDockMap != m_pVNCDockWidget && pDockMap != m_pSSHDockWidget && pDockMap != m_pShareDockWidget && pDockMap != m_pWebsiteDockWidget) {
			disconnect(pDockMap->GetThumbWidget(), SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onCloseWebEngineViewClicked(QString& key, bool isAxWidget)));
			disconnect(pDockMap->GetThumbWidget(), SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onOpenWebEngineViewClicked(QString& key, bool isAxWidget)));

			pDockMap->GetThumbWidget()->StopThumb();
			pDockMap->deleteLater();
		}
	}
	m_dockMaps.clear();
	m_dockMaps.insert(ui->listWidget->item(3)->whatsThis(), m_pRdpsDockWidget);
	m_dockMaps.insert(ui->listWidget->item(5)->whatsThis(), m_pVNCDockWidget);
	m_dockMaps.insert(ui->listWidget->item(6)->whatsThis(), m_pSSHDockWidget);
	m_dockMaps.insert(ui->listWidget->item(4)->whatsThis(), m_pShareDockWidget);
	m_dockMaps.insert(ui->listWidget->item(8)->whatsThis(), m_pWebsiteDockWidget);

	QList<QWidget*> widgetList = axWidgetManage.getWidgetList();
	for (int i = 0; i < widgetList.count(); i++) {
		QWidget *pWidget = widgetList.at(i);
		if (pWidget->inherits("OneCloudWidget")) {
			axWidgetManage.closeAxWidget(pWidget->whatsThis());
		}
		else {
			axWidgetManage.closeWebEngineView(pWidget->whatsThis());
		}
	}
	for (int i = 0;i < infoListWidgetList.size();i++) {
		infoListWidgetList[i]->clearInfoWidget();
		infoListWidgetList[i]->hide();
	}
	axWidgetManage.clearWidget();
	mstscRdpInfoMap.clear();
	websitedataMap.clear();
#else
	oneCloudManage.clear();
#endif // !ONE_CLOUD_USE

	
	//remoteDesktopNum = 0;
	//ui->sysFrame->hide();
	m_pDockMenu->hide();

	ui->horizontalFrame->setGeometry(0, 0, ui->verticalFrame->width(), this->size().height());
	ui->horizontalFrame->setAutoHideSize(QSize(ui->verticalFrame->width(), this->size().height()));
	ui->horizontalFrame->hideWidget();
	ui->horizontalFrame->setAutoHide(false);
	//ui->widget_2->show();
	ui->loginWidget->setLogin(false);
	ui->loginWidget->setMaskShow(0);
	ui->listWidget->clearSelection();
	on_openLoginBtn_clicked();
	webSocket->abort();
	webSocket->start();

	QPalette palette;
	QPixmap bgImage(":/powerdudemain/Resources/u8.jpg");
	bgImage.scaled(ui->widget_3->size());//, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
	palette.setBrush(QPalette::Background, QBrush(bgImage.scaled(ui->widget_3->size())));
	ui->widget_3->setPalette(palette);

	SingleTon<Settings>::getInstance().beginGroup("Globa");
	int exit = SingleTon<Settings>::getInstance().value("exit", 0).toInt();
	SingleTon<Settings>::getInstance().endGroup();
	if (exit == 1) {
		close();
	}
}


void PowerdudeMain::on_openLoginBtn_clicked()
{
	ui->loginWidgetFrame->show();

	if (!ui->loginWidget->remember()) {
		ui->loginWidget->clearPassword();
	}
}

void PowerdudeMain::on_downBtn_clicked()
{
	ui->listWidget->verticalScrollBar()->setValue(ui->listWidget->verticalScrollBar()->value() + ui->listWidget->gridSize().height());
	//ui->listWidget->verticalScrollBar()->valueChanged(95);
}

//void PowerdudeMain::on_upBtn_clicked()
//{
//	ui->listWidget->verticalScrollBar()->setValue(ui->listWidget->verticalScrollBar()->value() - ui->listWidget->gridSize().height());
//}

void PowerdudeMain::onUnLock()
{
	Lock *pLock = qobject_cast<Lock *>(sender());

	disconnect(pLock, SIGNAL(unLock()), this, SLOT(onUnLock()));
	disconnect(pLock, SIGNAL(minClicked()), this, SLOT(showMinimized()));
	disconnect(pLock, SIGNAL(exitClicked()), this, SLOT(on_exitBtn_clicked()));
	pLock->hide();
	pLock->close();

	this->show();
	this->update();
	this->activateWindow();
	
}

void PowerdudeMain::on_addBtn_clicked()
{
	ui->loginWidgetFrame->show();

	if (!ui->loginWidget->remember()) {
		ui->loginWidget->clearPassword();
	}

	ui->loginWidget->setMaskShow(this->webSocket->isLogin()?1:2);
}

void PowerdudeMain::onTextReceived(MstscRdpInfo & info, QString &msg)
{
	//if (isConnect)return;
	//setConnectInfo(info);
	//connectRemoteSystem();
	//ui->widget_2->hide();
	//ui->widget->hide();
	//ui->webEngineView->hide();

	ThumbWidget* thumb = 0;
	QString key = QString("%1@%2:%3").arg(info.username).arg(info.fulladdress).arg(info.port);
	//key.sprintf("%s@%s:%d", qPrintable(info.username), qPrintable(info.fulladdress), info.port);
	if (info.type == 3) {
		key = QString("%1@%2:%3%4").arg(info.username).arg(info.fulladdress).arg(info.port).arg(info.name);
	}
	//add remoteDesktopFrame
	switch (info.type) {
	case 0:
	case 1:
	case 2:
		if (mstscRdpInfoMap2.contains(key) || mstscRdpInfoMap.contains(key)) {
			OneCloudMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("桌面已存在，连接失败。"));
			return;
		}
        if (info.desktopType == RDPDestkopType::P2PDesktop) {
            key = info.p2pId;
 //           webSocket->m_loginDataHandler->getP2PDesktopIpPort(info);
 //           qDebug() << info.port;
        }
        // 判断是否需要二次认证
        if (!webSocket->m_loginDataHandler->hostxiaohui.isEmpty()) {
            secondScanWebSocket->start();
            bool loginResult = false;
            SingleTon<Settings>::getInstance().beginGroup("Globa");
            QString appccHost = SingleTon<Settings>::getInstance().value("website", "https//appcc.pispower.com").toString();
            SingleTon<Settings>::getInstance().endGroup();

            QString t = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
            QString url = QString("%1%2%3%4%5%6%7").arg(appccHost).arg("/s?T=28&k=").arg(secondScanWebSocket->_uuid).arg("&p=").arg(webSocket->_uuid).arg("&t=").arg(t);

            QDialog dlg(this);
            QTimer *pTimer = new QTimer(this);
            pTimer->setSingleShot(true);
            bool bTimeout = false;
            connect(pTimer, &QTimer::timeout, this, [&]() mutable {
                // 超时没有连接，关闭窗口和定时器
                bTimeout = true;
                if (pTimer != Q_NULLPTR) {
                    qDebug() << "timer delete 1";
                    pTimer->deleteLater();
                }
            });

            connect(this, &PowerdudeMain::onSecondScanFinished, this, [&](int result) mutable {
                if (result == 1){
                    QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("认证失败！"));
                }
                else {
                    loginResult = true;
                }

                dlg.close();
            });

            dlg.setGeometry(QRect(QApplication::desktop()->screenGeometry().width() / 2 - 110, QApplication::desktop()->screenGeometry().height() / 2 - 110, 220, 220));
            WebLabel *pLabel = new WebLabel(&dlg);
            pLabel->onShowUrl(url);
            pTimer->start(300000);

            int res = dlg.exec();

            if (bTimeout) {
                webSocket->onLoginComplete();  // XXX feedback appcc fail
                break;
            }

            if (!loginResult) { // do nothing
                return;
            }
            secondScanWebSocket->close();
        }

		if (mstscRdpInfoMap.contains(key) && mstscRdpInfoMap[key].type == 0) {

			InfoWidget* pInfoWidget = ui->remoteDesktopFrame->getInfoWidget(key);
			if (pInfoWidget != NULL) {
				pInfoWidget->setType(info.desktopType);
				pInfoWidget->setName(info.name);
			}
		}
		mstscRdpInfoMap.insert(key, info);
		ui->remoteDesktopFrame->addInfoWidget(key, info.name, info.type, InfoWidget::ToConnecting)->showX();
		ui->listWidget->item(3)->setSelected(true);
		axWidgetManage.openAxWidget(key, info, ui->listWidget->item(3)->icon());
		break;
	case 3:
		if (mstscRdpInfoMap.contains(key) && mstscRdpInfoMap[key].type == 0) {

			InfoWidget* pInfoWidget = ui->fileFrame->getInfoWidget(key);
			if (pInfoWidget != NULL) {
				pInfoWidget->setType(info.desktopType);
				pInfoWidget->setName(info.name);
			}
		}
		mstscRdpInfoMap.insert(key, info);
		//ui->fileFrame->addInfoWidget(key, info.name, info.type, InfoWidget::ToConnecting)->showX();
		ui->fileFrame->addInfoWidget(key, QString("%1@%2:%3").arg(info.username).arg(info.fulladdress).arg(info.port), info.name, info.type, true, InfoWidget::ToConnecting)->showX();
		ui->listWidget->item(10)->setSelected(true);
		axWidgetManage.openAxWidget(key, info, ui->listWidget->item(10)->icon());
		break;
	case 4:
	{
		key = QString("%1@%2").arg(info.name).arg(info.desktopLiveId);
		//qDebug() << key;
		int type = 40;
		if (!info.host) {
			type = 41;
		}
		mstscRdpInfoMap.insert(key, info);
		if (ui->shareDesktopFrame->addInfoWidget(key, info.subject, type, InfoWidget::Disconnecting)->getStatus() == InfoWidget::Disconnecting) {
            ui->shareDesktopFrame->getInfoWidget(key)->setConnectStatus(InfoWidget::ToConnecting);
			//ui->shareDesktopFrame->getInfoWidget(key)->hideX();
			pbVncSshDesktop->getDesktopLiveUrl(key, msg);
			ui->listWidget->item(4)->setSelected(true);
			axWidgetManage.openWebEngineView(key, QString(), ui->listWidget->item(4)->icon());
		}
		webSocket->onLoginComplete();
		break;
	}

    case 5:
        bool isAutoOpen;
        SingleTon<Settings>::getInstance().beginGroup("File");
        isAutoOpen = SingleTon<Settings>::getInstance().value("fileOpen", false).toBool();
        SingleTon<Settings>::getInstance().endGroup();

        if (isAutoOpen) {
            QString fileurl = QString("%1%2").arg("file:").arg(msg);
            QDesktopServices::openUrl(fileurl);
        }
        else {
            //QMessageBox::warning(NULL, QStringLiteral("提示"), QStringLiteral("接收文件完成！"));
			OneCloudMessageBox::information(this, QStringLiteral("接收文件完成！"));

        }
        webSocket->onLoginComplete();
        break;

	case 6:
		mstscRdpInfoMap.insert(key, info);
		if (ui->SSHFrame->addInfoWidget(key, info.name, info.type, InfoWidget::Disconnecting)->getStatus() == InfoWidget::Disconnecting) {
			ui->SSHFrame->getInfoWidget(key)->setConnectStatus(InfoWidget::ToConnecting);
			pbVncSshDesktop->getLoginUrl(key, msg);
			ui->listWidget->item(6)->setSelected(true);
			thumb = axWidgetManage.openWebEngineView(key, QString(), ui->listWidget->item(6)->icon());
			if (thumb != 0) {
				if (!m_sshThumbsMap.contains(key)) {
					m_sshThumbsMap.insert(key, thumb);
				}
				connect(thumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
				connect(thumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
				thumb->setWhatsThis(key);
				thumb->SetTitle(key);
				thumb->setFixedHeight(172);
				QListWidgetItem *alarmItem = new QListWidgetItem(m_pListWidgetSSH);
				thumb->SetItem(alarmItem);
				alarmItem->setSizeHint(QSize(222, 172));
				m_pListWidgetSSH->addItem(alarmItem);
				m_pListWidgetSSH->setItemWidget(alarmItem, thumb);

				int iHeight = 174 * m_pListWidgetSSH->count();
				m_pListWidgetSSH->resize(222, iHeight);
				m_pListWidgetSSH->update();
				QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(6));
				int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
				m_pListWidgetSSH->resize(222, iHeight);
				m_pSSHDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			}
		}
		webSocket->onLoginComplete();
		break;
	case 8:
		mstscRdpInfoMap.insert(key, info);
		if (ui->VNCFrame->addInfoWidget(key, info.name, info.type, InfoWidget::Disconnecting)->getStatus() == InfoWidget::Disconnecting) {
			ui->VNCFrame->getInfoWidget(key)->setConnectStatus(InfoWidget::ToConnecting);
			pbVncSshDesktop->getLoginUrl(key, msg);
			ui->listWidget->item(5)->setSelected(true);
			thumb = axWidgetManage.openWebEngineView(key, QString(), ui->listWidget->item(5)->icon());
			if (thumb != 0) {
				if (!m_vncThumbsMap.contains(key)) {
					m_vncThumbsMap.insert(key, thumb);
				}
				connect(thumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
				connect(thumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
				thumb->setWhatsThis(key);
				thumb->SetTitle(key);
				thumb->setFixedHeight(172);
				QListWidgetItem *alarmItem = new QListWidgetItem(m_pListWidgetVNC);
				thumb->SetItem(alarmItem);
				alarmItem->setSizeHint(QSize(222, 172));
				m_pListWidgetVNC->addItem(alarmItem);
				m_pListWidgetVNC->setItemWidget(alarmItem, thumb);

				int iHeight = 174 * m_pListWidgetVNC->count();
				m_pListWidgetVNC->resize(222, iHeight);
				m_pListWidgetVNC->update();
				QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(5));
				int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
				m_pListWidgetVNC->resize(222, iHeight);
				m_pVNCDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			}
		}
		webSocket->onLoginComplete();

		break;
	case 7:
		webSocket->onLoginComplete();
		ui->listWidget->item(0)->setSelected(true);
		thumb = axWidgetManage.openWebEngineView(ui->listWidget->item(0)->whatsThis(), msg, ui->listWidget->item(0)->icon());
		if (thumb != 0) {
			DockWidget* pDockWidget = 0;
			if (m_dockMaps.contains(ui->listWidget->item(0)->whatsThis())) {
				pDockWidget = m_dockMaps.find(ui->listWidget->item(0)->whatsThis()).value();
			}
			if (pDockWidget == 0) {
				pDockWidget = new DockWidget(ui->widget_3);
				connect(thumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onCloseWebEngineViewClicked(QString&, bool)));
				connect(thumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onOpenWebEngineViewClicked(QString&, bool)));
				if (!m_dockMaps.contains(ui->listWidget->item(0)->whatsThis())) {
					m_dockMaps.insert(ui->listWidget->item(0)->whatsThis(), pDockWidget);
				}
			}
			pDockWidget->SetThumbWidget(thumb);
		}
		break;
	case 14://空间
		webSocket->onLoginComplete();
		ui->listWidget->item(2)->setSelected(true);
		thumb = axWidgetManage.openWebEngineView(ui->listWidget->item(2)->whatsThis(), msg, ui->listWidget->item(2)->icon());
		if (thumb != 0) {
			DockWidget* pDockWidget = 0;
			if (m_dockMaps.contains(ui->listWidget->item(2)->whatsThis())) {
				pDockWidget = m_dockMaps.find(ui->listWidget->item(2)->whatsThis()).value();
			}
			if (pDockWidget == 0) {
				pDockWidget = new DockWidget(ui->widget_3);
				connect(thumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onCloseWebEngineViewClicked(QString&, bool)));
				connect(thumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onOpenWebEngineViewClicked(QString&, bool)));
				if (!m_dockMaps.contains(ui->listWidget->item(2)->whatsThis())) {
					m_dockMaps.insert(ui->listWidget->item(2)->whatsThis(), pDockWidget);
				}
			}
			pDockWidget->SetThumbWidget(thumb);
		}
		break;
	case 11:
		key = QString::number(info.websitedata.bizId);//info.websitedata.thirdpartyName;
		webSocket->onLoginComplete();
		mstscRdpInfoMap.insert(key, info);
		websitedataMap.insert(info.websitedata.accessUrl, key);
		ui->listWidget->item(8)->setSelected(true);
		if (ui->webFrame->addInfoWidget(info.websitedata.accessUrl, info.websitedata.thirdpartyName, info.type, InfoWidget::Disconnecting)->getStatus() == InfoWidget::Disconnecting) {
			ui->webFrame->getInfoWidget(info.websitedata.accessUrl)->setConnectStatus(InfoWidget::Connecting);
			thumb = axWidgetManage.openWebEngineView(info.websitedata.accessUrl, info.websitedata.accessUrl, ui->listWidget->item(8)->icon());
			if (thumb != 0) {
				if (!m_websiteThumbsMap.contains(info.websitedata.accessUrl)) {
					m_websiteThumbsMap.insert(info.websitedata.accessUrl, thumb);
				}
				connect(thumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
				connect(thumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
				thumb->setWhatsThis(info.websitedata.accessUrl);
				thumb->SetTitle(info.websitedata.accessUrl);
				thumb->setFixedHeight(172);
				QListWidgetItem *alarmItem = new QListWidgetItem(m_pListWidgetWebsite);
				thumb->SetItem(alarmItem);
				alarmItem->setSizeHint(QSize(222, 172));
				m_pListWidgetWebsite->addItem(alarmItem);
				m_pListWidgetWebsite->setItemWidget(alarmItem, thumb);

				int iHeight = 174 * m_pListWidgetWebsite->count();
				m_pListWidgetWebsite->resize(222, iHeight);
				m_pListWidgetWebsite->update();
				QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(8));
				int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
				m_pListWidgetWebsite->resize(222, iHeight);
				m_pWebsiteDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			}
		}
		
		break;

    case 16:
        // 进行二次认证， 获取扫码人的key和hostxiaohui，与认证信息的key和小慧号对比
        int result;

        if (!info.prekey.compare(webSocket->_uuid) 
            && !info.xiaohui.compare(webSocket->m_loginDataHandler->hostxiaohui)) 
        {
            // 回馈APPCC成功，且关闭二维码提示框，使用信号槽形式关闭对话框
            result = 0;
            secondScanWebSocket->onLoginComplete();
        }
        else
        {
            // 回馈APPCC失败，且关闭二维码提示框，使用信号槽形式关闭对话框
            result = 1;
            secondScanWebSocket->onLoginComplete();
        }
        emit onSecondScanFinished(result);

        break;

    case DESKTOP_GROUP_TYPE:
        key = info.desktopGroupData.bizId + "@" + QString::number(info.type) + "@" 
            + QString::number(info.desktopGroupData.count);
        mstscRdpInfoMap.insert(key, info);
        InfoWidget *infoWidget =ui->rdpGroupFrame->addInfoWidget(key, info.name, info.type, InfoWidget::ToConnecting);
        infoWidget->showCloseBtn();
        infoWidget->hideConnBtn();
        ui->listWidget->item(9)->setSelected(true);
        //axWidgetManage.openAxWidget(key, info, ui->listWidget->item(3)->icon());
        break;
	}


	//qDebug()<<ui->listWidget->children().at(3)->whatsThis();

	//isConnect = true;
}

void PowerdudeMain::onTextReceived(QList<MstscRdpInfo> &infoList)
{
	for (int i = 0;i < infoList.size();i++) {
		MstscRdpInfo info = infoList[i];
		QString key = QString("%1@%2:%3").arg(info.username).arg(info.fulladdress).arg(info.port);
        if (!info.desktopLiveId.isEmpty()) {
            key = QString("%1@%2").arg(info.name).arg(info.desktopLiveId);
        }
		if (info.type == 3) {
			key = QString("%1@%2:%3%4").arg(info.username).arg(info.fulladdress).arg(info.port).arg(info.name);
		}
        if (info.type == DESKTOP_GROUP_TYPE) {
            key = info.desktopGroupData.bizId + "@" + QString::number(info.type) + "@"
                + QString::number(info.desktopGroupData.count);
        }
		if (info.type==11) {
			key = QString::number(info.websitedata.bizId);//info.websitedata.accessUrl;
			if (!mstscRdpInfoMap.contains(key)) {
				mstscRdpInfoMap.insert(key, info);
				websitedataMap.insert(info.websitedata.accessUrl, key);
				ui->webFrame->addInfoWidget(info.websitedata.accessUrl, info.websitedata.thirdpartyName, info.type, InfoWidget::Disconnecting)->hideX();
			}
			continue;
		}
		//key.sprintf("%s@%s:%d", qPrintable(info.username), qPrintable(info.fulladdress), info.port);
		if (!mstscRdpInfoMap.contains(key) && ui->remoteDesktopFrame->getInfoWidget(key) == NULL) {
            if (info.desktopType == RDPDestkopType::P2PDesktop) {
                key = info.p2pId;
            }
            
			mstscRdpInfoMap.insert(key, info);
			switch (info.type) {
			case 0:
			case 1:
			case 2:
                if (info.desktopType != RDPDestkopType::DynamicCmdDesktop) {
                    ui->remoteDesktopFrame->addInfoWidget(key, info.name, info.desktopType, InfoWidget::Disconnecting)->hideX();
                }
				break;
			case 3:
				ui->fileFrame->addInfoWidget(key, QString("%1@%2:%3").arg(info.username).arg(info.fulladdress).arg(info.port), info.name, info.type, true, InfoWidget::ToConnecting)->hideX();
				//ui->fileFrame->addInfoWidget(key, info.name, info.desktopType, InfoWidget::Disconnecting)->hideX();
				break;
            case DESKTOP_GROUP_TYPE:
                ui->rdpGroupFrame->addInfoWidget(key, info.name, info.type, InfoWidget::Disconnecting)->hideX();
                break;
			case 6:
				ui->SSHFrame->addInfoWidget(key, info.name, info.type, InfoWidget::Disconnecting)->hideX();
				break;
			case 8:
				ui->VNCFrame->addInfoWidget(key, info.name, info.type, InfoWidget::Disconnecting)->hideX();
				break;
			case 4:
				int type = 40;
				if (!info.host) {
					type = 41;
				}
				ui->shareDesktopFrame->addInfoWidget(key, info.subject, type, InfoWidget::Disconnecting)->hideX();
				break;
			}
		}
		else if (mstscRdpInfoMap.contains(key) && mstscRdpInfoMap[key].type == 0) {
			mstscRdpInfoMap.remove(key);
			mstscRdpInfoMap.insert(key, info);

			InfoWidget* pInfoWidget = ui->remoteDesktopFrame->getInfoWidget(key);
			pInfoWidget->setType(info.type);
			pInfoWidget->setName(info.name);
		}
	}
}

void PowerdudeMain::onOneCloudDataReceived(int type, QJsonObject & jsonObject)
{
	OneCloudData* pOneCloudData = OneCloudDataFactory::getOneCloudDataFromJsonObject(type, jsonObject);
	QString key = pOneCloudData->createKey();
	if (!oneCloudDataMap.contains(key)) {
		oneCloudDataMap.insert(key, pOneCloudData);
	}

	if (controlPanelInfoMap.contains(type)) {
		controlPanelInfo& cpInfo = controlPanelInfoMap[type];
		if(cpInfo.pInfoListWidget!=NULL)cpInfo.pInfoListWidget->addInfoWidget(key, pOneCloudData->getData("url").toString(), pOneCloudData->getData("name").toString(), pOneCloudData->getData("type").toInt());
		OneCloudView* pView = OneCloudDataFactory::createOneCloudView(type, pOneCloudData);
		oneCloudManage.openOneCloudMainWindow(key, pOneCloudData->getData("name").toString(), pView, cpInfo.icon);
	}

}

void PowerdudeMain::onLogin()
{
	/*ui->widget_2->hide();
	ui->widget->hide();*/
	ui->loginWidgetFrame->hide();
	if (webSocket->isLogin()) {
		ui->remoteDesktopFrame->showUpdateBtn();
		ui->loginWidget->setLogin(true);

		//onTextReceived(cloudblkmgr->getDesktopList(webSocket->m_chatSvrInfo, "/business/user/desktop/list"));
		//onTextReceived(cloudblkmgr->getVncDesktopList(webSocket->m_chatSvrInfo, "/business/user/vnc/list"));

		cloudblkmgr->getDesktopList(webSocket->m_chatSvrInfo, "/business/user/desktop/list");
		cloudblkmgr->getVncDesktopList(webSocket->m_chatSvrInfo, "/business/user/vnc/list");
		cloudblkmgr->getSSHDesktopList(webSocket->m_chatSvrInfo, "/business/user/ssh/list");
		cloudblkmgr->getDesktopLiveList(webSocket->m_chatSvrInfo, "/business/user/desktop/play/list");
		cloudblkmgr->getWebsiteList(webSocket->m_chatSvrInfo, "/business/user/thirdparty/list");
		cloudblkmgr->getDesktopFileList(webSocket->m_chatSvrInfo, "/business/user/desktop/file/list");
        cloudblkmgr->getDesktopGroupList(webSocket->m_chatSvrInfo);
	}
	//ui->widget->setCloseEn(true);
	if (!ui->horizontalFrame->isAutoHide()) {
		QPalette palette;
		QPixmap bgImage(":/powerdudemain/Resources/u443.png");
		bgImage.scaled(ui->widget_3->size());//, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
		palette.setBrush(QPalette::Background, QBrush(bgImage.scaled(ui->widget_3->size())));
		ui->widget_3->setPalette(palette);

		ui->horizontalFrame->setAutoHide(true);
	}
}

void PowerdudeMain::onLocalLogin(MstscRdpInfo & info)
{
	QString key;
	key.sprintf("%s@%s:%d", qPrintable(info.username), qPrintable(info.fulladdress), info.port);
	if (ui->remoteDesktopFrame->getInfoWidget(key) == NULL && !mstscRdpInfoMap.contains(key)) {
		SingleTon<Settings>::getInstance().beginGroup("RemoteDesktop");
		SingleTon<Settings>::getInstance().setValue("ip", info.fulladdress);
		SingleTon<Settings>::getInstance().setValue("port", info.port);
		SingleTon<Settings>::getInstance().setValue("username", info.username);
		if (ui->loginWidget->remember()) {
			if (info.password.length() > 0) {
				QByteArray ary = info.password.toLatin1();
				xorData(ary);
				SingleTon<Settings>::getInstance().setValue("password", QString(ary));
			}
		}
		SingleTon<Settings>::getInstance().setValue("domain", info.domain);
		SingleTon<Settings>::getInstance().endGroup();
		ui->remoteDesktopFrame->hideUpdateBtn();
		onLogin();
		onTextReceived(info, QString());
	}
	else {
		//QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("本连接已存在"));
		OneCloudMessageBox::information(this, QStringLiteral("本连接已存在"));
	}
}

void PowerdudeMain::onClose()
{
	if (ui->horizontalFrame->isAutoHide()) {
		ui->loginWidgetFrame->hide();
	}
	else {
		close();
	}
}

void PowerdudeMain::onUserInfo(QString & url, QString & name, QString & mobile)
{
	userInfo.url = url;
	userInfo.name = name;
	userInfo.mobile = mobile;

	QNetworkAccessManager* manager = new QNetworkAccessManager(this);
	QUrl picture_url(url);
	QNetworkRequest request(picture_url);
	/*QSslConfiguration config;
	config.setPeerVerifyMode(QSslSocket::VerifyNone);
	config.setProtocol(QSsl::SslV3);
	request.setSslConfiguration(config);*/
	//request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/x-www-form-urlencoded"));
	connect(manager, SIGNAL(finished(QNetworkReply*)), this, SLOT(getPictrueSlot(QNetworkReply*)));
	manager->get(request)->ignoreSslErrors();
}

void PowerdudeMain::getPictrueSlot(QNetworkReply * reply)
{
	QVariant status = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute);
	QVariant redirectionTarget = reply->attribute(QNetworkRequest::RedirectionTargetAttribute);
	if (reply->error() == QNetworkReply::NoError)
	{
		QByteArray data_bytes = reply->readAll();
		QPixmap cur_pictrue;
		cur_pictrue.loadFromData(data_bytes);
		
		userInfo.pixmap = new QPixmap(cur_pictrue/*PixmapToRound(cur_pictrue,50)*/);
	}
	reply->deleteLater();
}

QPixmap PowerdudeMain::PixmapToRound(QPixmap &src, int radius)
{
	if (src.isNull()) {
		return QPixmap();
	}
	QSize size(2 * radius, 2 * radius);
	QBitmap mask(size);
	QPainter painter(&mask);
	painter.setRenderHint(QPainter::Antialiasing);
	painter.setRenderHint(QPainter::SmoothPixmapTransform);
	painter.fillRect(0, 0, size.width(), size.height(), Qt::white);
	painter.setBrush(QColor(0, 0, 0));
	painter.drawRoundedRect(0, 0, size.width(), size.height(), 99, 99);
	QPixmap image = src.scaled(size);
	image.setMask(mask);
	return image;
}

void PowerdudeMain::OnClose(QString& key)
{
	InfoListWidget* pInfoListWidget = infoListWidgetList[0];
	if (0 == pInfoListWidget) {
		return;
	}
	pInfoListWidget->removeInfoWidget(key);
}

void PowerdudeMain::onLoginComplete(QString &key)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	InfoWidget* pInfo;
	MstscRdpInfo info = mstscRdpInfoMap[key];
	if (info.type == 3) {
		pInfo = ui->fileFrame->getInfoWidget(key);
	}
	else {
		pInfo = ui->remoteDesktopFrame->getInfoWidget(key);
	}
	pInfo->setConnectStatus(InfoWidget::Connecting);//setStatusText(QStringLiteral("连接中"));
	OneCloudWidget* pOneCloudWidget = axWidgetManage.GetOneCloudWidget(key);
	if (pOneCloudWidget != 0) {
		HWND hWnd = pOneCloudWidget->GetAxHwnd();
		if (hWnd == 0) {
			return;
		}
		ThumbWidget* thumb = new ThumbWidget(hWnd, m_pListWidget);
		pOneCloudWidget->setThumbWidget(thumb);
		thumb->setWhatsThis(pOneCloudWidget->whatsThis());
		connect(thumb, &ThumbWidget::thumbCloseWidget, [&](QString& key, bool isAxWidget) {
			//axWidgetManage.closeAxWidget(key);
			InfoWidget* pInfoWidget = infoListWidgetList[0]->getInfoWidget(key);
			if (pInfoWidget != 0) {
				emit pInfoWidget->toClose();
			}
		});

		connect(thumb, &ThumbWidget::thumbOpenWidget, [&](QString& key, bool isAxWidget) {
			MstscRdpInfo info = mstscRdpInfoMap[key];
			axWidgetManage.openAxWidget(key, info, ui->listWidget->item(3)->icon());
		});
		thumb->SetTitle(pOneCloudWidget->whatsThis());
		thumb->setFixedHeight(172);
		QListWidgetItem *alarmItem = new QListWidgetItem(m_pListWidget);
		thumb->SetItem(alarmItem);
		alarmItem->setSizeHint(QSize(222, 172));
		m_pListWidget->addItem(alarmItem);
		m_pListWidget->setItemWidget(alarmItem, thumb);

		int iHeight = 174 * m_pListWidget->count();
		m_pListWidget->resize(222, iHeight);
		m_pListWidget->update();
		QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(3));
		int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
		m_pListWidget->resize(222, iHeight);
		m_pRdpsDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
	}
}

void PowerdudeMain::onDisconnected(QString &key)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;

	InfoWidget* pInfo;
	MstscRdpInfo info = mstscRdpInfoMap[key];
	if (info.type == 3) {
		pInfo = ui->fileFrame->getInfoWidget(key);
	}
	else {
		pInfo = ui->remoteDesktopFrame->getInfoWidget(key);
	}
	if (pInfo != NULL)pInfo->setConnectStatus(InfoWidget::Disconnecting);//setStatusText(QString());
	if (ui->remoteDesktopFrame->isHidden()) {
		int verticalFrameW = 100;
		if (ui->horizontalFrame->getShow() > 0) {
			ui->remoteDesktopFrame->hide();
			ui->horizontalFrame->setGeometry(0, 0, verticalFrameW, this->size().height());
		}
	}
	OneCloudWidget* pOneCloudWidget = axWidgetManage.GetOneCloudWidget(key);
	if (pOneCloudWidget != 0) {
		ThumbWidget* thumb = (ThumbWidget*)pOneCloudWidget->getThumbWidget();
		if (thumb != 0 && thumb->GetItem() != NULL) {
			int iRow = m_pListWidget->row(thumb->GetItem());
			m_pListWidget->removeItemWidget(thumb->GetItem());
			thumb->SetItem(NULL);
			if (m_pTabIconDialog != 0) {
				ThumbWidget* tabThumb = m_pTabIconDialog->getThumbWidget(thumb->GetHwnd());
				if (tabThumb != 0) {
					tabThumb->SetHwnd(0);
					tabThumb->SetWidget(pOneCloudWidget);
				}
			}
			m_pListWidget->takeItem(iRow);
			int iHeight = 174 * m_pListWidget->count();
			m_pListWidget->resize(222, iHeight);
			m_pListWidget->update();
			QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(3));
			int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
			m_pListWidget->resize(222, iHeight);
			m_pRdpsDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			pOneCloudWidget->setThumbWidget(NULL);
			thumb->deleteLater();
		}
	}
}


void PowerdudeMain::onAuthenticationWarningDismissed()
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
}

void PowerdudeMain::onAuthenticationWarningDisplayed()
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
}

void PowerdudeMain::onLogonError(int error)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
}

void PowerdudeMain::onNetworkStatusChanged(uint qualityLevel, int bandwidth, int rtt)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	qDebug() << "qualityLevel:" << qualityLevel << "bandwidth:" << bandwidth << "rtt:" << rtt;
}

void PowerdudeMain::onTimerOut()
{
	int index = SingleTon<Settings>::getInstance().oneValue("Globa", "autoLock").toInt();
	if (webSocket->isLogin()&& index==1&&(!(windowState()&Qt::WindowMinimized))) {
		
		if (!this->isHidden()) {
			on_lockBtn_clicked();
		}
	}
}

void PowerdudeMain::onLoginTimerOut()
{
	if (webSocket->isLogin()) {
		HttpClient(webSocket->m_chatSvrInfo.chatServerApi + "/business/user/token/check")
			.addParam("token", webSocket->m_chatSvrInfo.token)
			.addHeader("Oc_Platform_AppVersion", "3.0.0")
			.addHeader("Oc_Chatserver_MinVersion", "3.0.0")
			.get([=](const QString &response) mutable {
			qDebug() << response;

			QJsonParseError jsonerror;
			QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

			if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
				QJsonObject jsonObject = document.object();
				int code = document.object().value("code").toInt();
				if (0 != code) {
					OneCloudMessageBox::information(this, QStringLiteral("下线通知"), QStringLiteral("账号在其他地方登入。"));
					on_exitBtn_clicked();
				}
			}
		});
	}
}

void PowerdudeMain::onShortcut(const QString & key)
{
	if (SingleTon<Settings>::getInstance().oneValue("Globa", "hotKeyEn", true).toBool()) {
		if ("Alt+W" == key) {
			onAltAndTab();
		}
		else if ("Alt+A" == key) {
			onAltAndA();
		}
		else if ("Alt+D" == key) {
			onAltAndD();
		}
		else if ("Alt+Q" == key) {
			onAltAndDel();
		}
		else if ("Alt+G" == key) {
			onAltAndG();
		}
		else if ("Ctrl+Alt+A" == key) {
			onCtrlAndA();
		}
		else if ("AltUp" == key) {
			onAltUp();
		}
	}
}

void PowerdudeMain::onAltAndTab()
{
	if (!ui->loginWidgetFrame->isHidden()) {
		return;
	}
	if (QSimpleScreenShot::instance()->isScreenShot()) {
		return;
	}
	if (m_pTabIconDialog == 0) {
		m_pTabIconDialog = new TabIconDialog(g_pfBitBlt,this);
		m_pTabIconDialog->setWindowsList(axWidgetManage.getWidgetList(), axWidgetManage.getNextWidget());
		m_pTabIconDialog->exec();
		delete m_pTabIconDialog;
		m_pTabIconDialog = 0;
	}
	else {
		m_pTabIconDialog->toNext();
	}
}

void PowerdudeMain::onAltAndD()
{
	//qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	axWidgetManage.downWidget();
}

void PowerdudeMain::onAltAndA()
{
	if (waitHide) {
		waitHide = false;
		m_pTimer->stop();
		ui->hideLabel->hide();
		ui->horizontalFrame->ToHideWidget();
	}
}

void PowerdudeMain::onAltUp()
{
	if (!ui->loginWidgetFrame->isHidden()) {
		return;
	}
	if (m_pTabIconDialog != 0) {
		axWidgetManage.changeWidget(m_pTabIconDialog->getIndex());
		m_pTabIconDialog->clearThumbList();
		m_pTabIconDialog->accept();
	}
}

void PowerdudeMain::onAltAndDel()
{
#ifndef ONE_CLOUD_USE
	QString key = axWidgetManage.getFrontWidgetKey();
	QString oldKey = key;
	if (!key.isNull()) {
		if (websitedataMap.contains(key)) {
			key = websitedataMap[key];
		}
		else {
			ui->horizontalFrame->unRegisterDocketWidget();
			DockWidget* pDockWidget = 0;
			if (m_dockMaps.contains(key)) {
				pDockWidget = m_dockMaps.find(key).value();
			}
			if (pDockWidget != 0) {
				disconnect(pDockWidget->GetThumbWidget(), SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onCloseWebEngineViewClicked(QString& key, bool isAxWidget)));
				disconnect(pDockWidget->GetThumbWidget(), SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onOpenWebEngineViewClicked(QString& key, bool isAxWidget)));
				m_dockMaps.remove(key);
				pDockWidget->deleteLater();
			}
		}
		if (mstscRdpInfoMap.contains(key)) {
			MstscRdpInfo info = mstscRdpInfoMap[key];
			if (info.type <= 2) {
				onCloseAxWidget(key);
				if (info.type == 0) {
					ui->remoteDesktopFrame->removeInfoWidget(key);
					mstscRdpInfoMap.remove(key);
				}
				else {
					ui->remoteDesktopFrame->getInfoWidget(key)->hideX();
				}
			}
			else {
				onCloseWebEngineView(oldKey);
				switch (info.type) {
				case 4:
					ui->shareDesktopFrame->getInfoWidget(key)->hideX();
					break;
				case 6:
					ui->SSHFrame->getInfoWidget(key)->hideX();
					break;
				case 8:
					ui->VNCFrame->getInfoWidget(key)->hideX();
					break;
				case 11:
					ui->webFrame->getInfoWidget(oldKey)->hideX();
					break;
				}
			}
		}
		else {
			onCloseWebEngineView(key);
		}
	}
#else
	oneCloudManage.closeFrontWindow();

#endif
}

void PowerdudeMain::onOpenAxWidget(QString &key)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	//axWidgetManage.openAxWidget(key);
	InfoWidget* pInfoWidget;
	MstscRdpInfo info = mstscRdpInfoMap[key];
	if (info.type == 3) {
		pInfoWidget = ui->fileFrame->getInfoWidget(key);
    }
    else if (info.type == DESKTOP_GROUP_TYPE) {
        pInfoWidget = ui->rdpGroupFrame->getInfoWidget(key);
    }
	else {
		pInfoWidget = ui->remoteDesktopFrame->getInfoWidget(key);
	}
	
	if (mstscRdpInfoMap.contains(key)) {
		if (pInfoWidget != NULL && !pInfoWidget->isXBtnShow()) {
            if (DESKTOP_GROUP_TYPE == info.type) {
                pInfoWidget->showCloseBtn();
            }
            else {
                pInfoWidget->showX();
            }
			
			pInfoWidget->setConnectStatus(InfoWidget::ToConnecting);
		}

		MstscRdpInfo info = mstscRdpInfoMap[key];
		//mstscRdpInfoMap.remove(key);
        if (DESKTOP_GROUP_TYPE == info.type) {  // 点击桌面组的InfoWidget, 且打开桌面列表连接
            for (int i = 0; i < mstscRdpInfoMap[key].desktopKeyList.count(); i++) {
                QString desktopKey = mstscRdpInfoMap[key].desktopKeyList.at(i);
                MstscRdpInfo desktopinfo = mstscRdpInfoMap[desktopKey];
                InfoWidget* desktopInfoWidget = ui->remoteDesktopFrame->getInfoWidget(desktopKey);
                if (desktopInfoWidget) {
                    desktopInfoWidget->showX();
                }
                axWidgetManage.openAxWidget(desktopKey, desktopinfo, ui->listWidget->item(3)->icon());
				//desktopInfoWidget->setConnectStatus(InfoWidget::ToConnecting);
            }
        }
        else {
            axWidgetManage.openAxWidget(key, info, ui->listWidget->item(3)->icon());
			//pInfoWidget->setConnectStatus(InfoWidget::ToConnecting);
        }
		if (info.type == 3) {
			ui->listWidget->item(10)->setSelected(true);
		}
        else if (DESKTOP_GROUP_TYPE == info.type) {
            ui->listWidget->item(3)->setSelected(true);
            ui->listWidget->item(9)->setSelected(true);
        }
		else {
			ui->listWidget->item(3)->setSelected(true);
		}
	}
	else {
		axWidgetManage.openAxWidget(key);
		//pInfoWidget->setConnectStatus(InfoWidget::ToConnecting);
	}
}

void PowerdudeMain::onConnectAxWidget()
{
	InfoWidget *pInfoWidget = qobject_cast<InfoWidget *>(sender());
	if (pInfoWidget->getStatus() == InfoWidget::Connecting) {
		axWidgetManage.disconnectAxWidget(pInfoWidget->getKey());
	}
	else if (pInfoWidget->getStatus() == InfoWidget::Disconnecting) {
		pInfoWidget->setConnectStatus(InfoWidget::ToConnecting);

		axWidgetManage.connectAxWidget(pInfoWidget->getKey());
	}
}

void PowerdudeMain::onConnectAxWidget(QString &key)
{
	//InfoWidget *pInfoWidget = qobject_cast<InfoListWidget *>(sender())->getInfoWidget(key);//ui->remoteDesktopFrame->getInfoWidget(key);
	InfoWidget* pInfoWidget;
	MstscRdpInfo info = mstscRdpInfoMap[key];
	if (info.type == 3) {
		pInfoWidget = ui->fileFrame->getInfoWidget(key);
	}
	else {
		pInfoWidget = ui->remoteDesktopFrame->getInfoWidget(key);
	}
	if (pInfoWidget == NULL)return;
	if (pInfoWidget->getStatus() == InfoWidget::Connecting) {
		axWidgetManage.disconnectAxWidget(key);
	}
	else if (pInfoWidget->getStatus() == InfoWidget::Disconnecting) {
		pInfoWidget->setConnectStatus(InfoWidget::ToConnecting);
		pInfoWidget->showX();

		if (mstscRdpInfoMap.contains(key)) {
			MstscRdpInfo info = mstscRdpInfoMap[key];
            if (info.desktopType == RDPDestkopType::P2PDesktop) {
                m_pLoginDataHandler->getP2PDesktopIpPort(info);
            }

            if (info.desktopType == RDPDestkopType::USBDesktop) {
                if (!m_pLoginDataHandler->usbAuth(info)) {
                    return;
                }
            }

			//mstscRdpInfoMap.remove(key);
			axWidgetManage.openAxWidget(key, info, ui->listWidget->item(3)->icon());
		}
		else {
			axWidgetManage.connectAxWidget(key);
		}
	}
}

void PowerdudeMain::onCloseAxWidget(QString&key)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	InfoWidget* pInfoWidget;
	MstscRdpInfo info = mstscRdpInfoMap[key];
	if (info.type == 3) {
		pInfoWidget = ui->fileFrame->getInfoWidget(key);
	}
    else if(info.type == DESKTOP_GROUP_TYPE) {
        pInfoWidget = ui->rdpGroupFrame->getInfoWidget(key);
    }
	else {
		pInfoWidget = ui->remoteDesktopFrame->getInfoWidget(key);
	}
	
    if (info.type == DESKTOP_GROUP_TYPE) {
        if (mstscRdpInfoMap.contains(key)){
            for (int i = 0; i < mstscRdpInfoMap[key].desktopKeyList.size(); i++) {
                QString desktopKey = mstscRdpInfoMap[key].desktopKeyList.at(i);

                // 获取对应桌面的InfoWidget进行关闭
                InfoWidget* desktopInfoWidget = ui->remoteDesktopFrame->getInfoWidget(desktopKey);
                if (desktopInfoWidget) {
                    desktopInfoWidget->setConnectStatus(InfoWidget::Disconnecting);
                    desktopInfoWidget->hideX();
                }

                // 关闭桌面
                axWidgetManage.closeAxWidget(desktopKey);
            }
        }
        
    }
    else {
        axWidgetManage.closeAxWidget(key);
    }
	if (/*mstscRdpInfoMap[key].type == 0*/pInfoWidget == NULL) {
		mstscRdpInfoMap.remove(key);
	}
}

void PowerdudeMain::onOpenWebEngineView(QString&key)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	InfoWidget *pInfoWidget = qobject_cast<InfoListWidget *>(sender())->getInfoWidget(key);
	QString strOrgKey = key;
	if (websitedataMap.contains(key)) {
		key = websitedataMap[key];
	}
	if (mstscRdpInfoMap.contains(key) && pInfoWidget != NULL) {
		pInfoWidget->showX();
		MstscRdpInfo info = mstscRdpInfoMap[key];
		ThumbWidget* thumb = 0;
		QListWidget* pDestListWidget = 0;
		DockWidget* pDestDockWidget = 0;
		int iIndex = 6;
		if (info.type == 6) {
			if (!axWidgetManage.contains(key)) {
				pbVncSshDesktop->getLoginUrl(key, info.msg);
			}
			ui->listWidget->item(6)->setSelected(true);
			thumb = axWidgetManage.openWebEngineView(key, QString(), ui->listWidget->item(6)->icon());
			pDestListWidget = m_pListWidgetSSH;
			pDestDockWidget = m_pSSHDockWidget;
			iIndex = 6;
			if (thumb != 0) {
				if (!m_sshThumbsMap.contains(key)) {
					m_sshThumbsMap.insert(key, thumb);
				}
			}
		}
		else if (info.type == 8) {
			if (!axWidgetManage.contains(key)) {
				pbVncSshDesktop->getLoginUrl(key, info.msg);
			}
			ui->listWidget->item(5)->setSelected(true);
			thumb = axWidgetManage.openWebEngineView(key, QString(), ui->listWidget->item(5)->icon());
			pDestListWidget = m_pListWidgetVNC;
			pDestDockWidget = m_pVNCDockWidget;
			iIndex = 5;
			if (thumb != 0) {
				if (!m_vncThumbsMap.contains(key)) {
					m_vncThumbsMap.insert(key, thumb);
				}
			}
		}
		else if (info.type == 4) {
			if (!(!info.host&& info.hostStatus != 2)) {
				if (!axWidgetManage.contains(key)) {
                    cloudblkmgr->constructSingleDesktopLiveData(webSocket->m_chatSvrInfo, info.desktopLiveId, key);
				}
				ui->listWidget->item(4)->setSelected(true);
				thumb = axWidgetManage.openWebEngineView(key, QString(), ui->listWidget->item(4)->icon());
				iIndex = 4;
				pDestListWidget = m_pListWidgetShare;
				pDestDockWidget = m_pShareDockWidget;
				if (thumb != 0) {
					if (!m_shareThumbsMap.contains(key)) {
						m_shareThumbsMap.insert(key, thumb);
					}
				}
			}
			else {
				pInfoWidget->hideX();
			}
		}
		else if (info.type == 11) {
			ui->listWidget->item(8)->setSelected(true);
			pInfoWidget->setConnectStatus(InfoWidget::ConnectStatus::Connecting);
			QString kye2;
			if (websitedataMap.contains(key)) {
				kye2 = websitedataMap[key];
			}
			thumb = axWidgetManage.openWebEngineView(info.websitedata.accessUrl, info.websitedata.accessUrl, ui->listWidget->item(8)->icon());
			iIndex = 8;
			pDestListWidget = m_pListWidgetWebsite;
			pDestDockWidget = m_pWebsiteDockWidget;
			if (thumb != 0) {
				if (!m_websiteThumbsMap.contains(strOrgKey)) {
					m_websiteThumbsMap.insert(strOrgKey, thumb);
				}
			}
			key = strOrgKey;
		}

		if (thumb != 0 && pDestListWidget!= 0  && pDestDockWidget != 0) {
				connect(thumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
				connect(thumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
				thumb->setWhatsThis(key);
				thumb->SetTitle(key);
				thumb->setFixedHeight(172);
				QListWidgetItem *alarmItem = new QListWidgetItem(pDestListWidget);
				thumb->SetItem(alarmItem);
				alarmItem->setSizeHint(QSize(222, 172));
				pDestListWidget->addItem(alarmItem);
				pDestListWidget->setItemWidget(alarmItem, thumb);

				int iHeight = 174 * pDestListWidget->count();
				pDestListWidget->resize(222, iHeight);
				pDestListWidget->update();
				QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(iIndex));
				int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
				pDestListWidget->resize(222, iHeight);
				pDestDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			}
	}
}

void PowerdudeMain::onConnectWebEngineView(QString&key)
{
	InfoWidget *pInfoWidget = qobject_cast<InfoListWidget *>(sender())->getInfoWidget(key);//ui->remoteDesktopFrame->getInfoWidget(key);
	QString strOrgKey = key;
	if (websitedataMap.contains(key)) {
		key = websitedataMap[key];
	}
	if (mstscRdpInfoMap.contains(key) && pInfoWidget != NULL) {
		MstscRdpInfo info = mstscRdpInfoMap[key];
		/*if (info.type == 11) {
			return;
		}*/
		if (pInfoWidget->getStatus() == InfoWidget::Connecting) {
			if (info.type == 11) {
				axWidgetManage.setUrl(info.websitedata.accessUrl, QString());
			}
			else {
				axWidgetManage.setUrl(key, QString());
			}
			pInfoWidget->setConnectStatus(InfoWidget::Disconnecting);
		}
		else if (pInfoWidget->getStatus() == InfoWidget::Disconnecting) {
			pInfoWidget->showX();
			pInfoWidget->setConnectStatus(InfoWidget::ToConnecting);
			ThumbWidget* thumb = 0;
			QListWidget* pDestListWidget = 0;
			DockWidget* pDestDockWidget = 0;
			int iIndex = 6;
			if (info.type == 6) {
				pbVncSshDesktop->getLoginUrl(key, info.msg);
				thumb = axWidgetManage.openWebEngineView(key, QString(), ui->listWidget->item(6)->icon());
				pDestListWidget = m_pListWidgetSSH;
				pDestDockWidget = m_pSSHDockWidget;
				iIndex = 6;
				if (thumb != 0) {
					if (!m_sshThumbsMap.contains(key)) {
						m_sshThumbsMap.insert(key, thumb);
					}
				}
			}
			else if (info.type == 8) {
				pbVncSshDesktop->getLoginUrl(key, info.msg);
				thumb = axWidgetManage.openWebEngineView(key, QString(), ui->listWidget->item(5)->icon());
				pDestListWidget = m_pListWidgetVNC;
				pDestDockWidget = m_pVNCDockWidget;
				iIndex = 5;
				if (thumb != 0) {
					if (!m_vncThumbsMap.contains(key)) {
						m_vncThumbsMap.insert(key, thumb);
					}
				}
			}
			else if (info.type == 4) {
				//thumb = axWidgetManage.openWebEngineView(key, QString(), ui->listWidget->item(5)->icon());
				if (!(!info.host&& info.hostStatus != 2)) {
                    cloudblkmgr->constructSingleDesktopLiveData(webSocket->m_chatSvrInfo, info.desktopLiveId, key);
					thumb = axWidgetManage.openWebEngineView(key, QString(), ui->listWidget->item(5)->icon());
				}
				else {
					pInfoWidget->hideX();
				}
				pDestListWidget = m_pListWidgetShare;
				pDestDockWidget = m_pShareDockWidget;
				iIndex = 4;
				if (thumb != 0) {
					if (!m_shareThumbsMap.contains(key)) {
						m_shareThumbsMap.insert(key, thumb);
					}
				}
			}
			else if (info.type == 11) {
				pInfoWidget->setConnectStatus(InfoWidget::ConnectStatus::Connecting);
				thumb = axWidgetManage.openWebEngineView(info.websitedata.accessUrl, info.websitedata.accessUrl, ui->listWidget->item(8)->icon());
				axWidgetManage.setUrl(info.websitedata.accessUrl, info.websitedata.accessUrl);
				iIndex = 8;
				pDestListWidget = m_pListWidgetWebsite;
				pDestDockWidget = m_pWebsiteDockWidget;
				if (thumb != 0) {
					if (!m_websiteThumbsMap.contains(strOrgKey)) {
						m_websiteThumbsMap.insert(strOrgKey, thumb);
					}
				}
				key = strOrgKey;
			}
			if (thumb != 0 && pDestListWidget!= 0  && pDestDockWidget != 0) {
				connect(thumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
				connect(thumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
				thumb->setWhatsThis(key);
				thumb->SetTitle(key);
				thumb->setFixedHeight(172);
				QListWidgetItem *alarmItem = new QListWidgetItem(pDestListWidget);
				thumb->SetItem(alarmItem);
				alarmItem->setSizeHint(QSize(222, 172));
				pDestListWidget->addItem(alarmItem);
				pDestListWidget->setItemWidget(alarmItem, thumb);

				int iHeight = 174 * pDestListWidget->count();
				pDestListWidget->resize(222, iHeight);
				pDestListWidget->update();
				QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(iIndex));
				int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
				pDestListWidget->resize(222, iHeight);
				pDestDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
			}
		}
	}
}

void PowerdudeMain::onCloseWebEngineView(QString&key)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	ThumbWidget* pThumb = 0;
	if (m_sshThumbsMap.contains(key)) {
		pThumb = m_sshThumbsMap.find(key).value();
		m_sshThumbsMap.remove(key);
		disconnect(pThumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
		disconnect(pThumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
		int iRow = m_pListWidgetSSH->row(pThumb->GetItem());
		m_pListWidgetSSH->removeItemWidget(pThumb->GetItem());
		pThumb->SetItem(NULL);
		m_pListWidgetSSH->takeItem(iRow);
		int iHeight = 174 * m_pListWidgetSSH->count();
		m_pListWidgetSSH->resize(222, iHeight);
		m_pListWidgetSSH->update();
		QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(6));
		int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
		m_pListWidgetSSH->resize(222, iHeight);
		m_pSSHDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
		pThumb->deleteLater();
	}
	if (m_vncThumbsMap.contains(key)) {
		pThumb = m_vncThumbsMap.find(key).value();
		m_vncThumbsMap.remove(key);
		disconnect(pThumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
		disconnect(pThumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
		int iRow = m_pListWidgetVNC->row(pThumb->GetItem());
		m_pListWidgetVNC->removeItemWidget(pThumb->GetItem());
		pThumb->SetItem(NULL);
		m_pListWidgetVNC->takeItem(iRow);
		int iHeight = 174 * m_pListWidgetVNC->count();
		m_pListWidgetVNC->resize(222, iHeight);
		m_pListWidgetVNC->update();
		QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(5));
		int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
		m_pListWidgetVNC->resize(222, iHeight);
		m_pVNCDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
		pThumb->deleteLater();
	}
	if (m_shareThumbsMap.contains(key)) {
		pThumb = m_shareThumbsMap.find(key).value();
		m_shareThumbsMap.remove(key);
		disconnect(pThumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
		disconnect(pThumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));
		int iRow = m_pListWidgetShare->row(pThumb->GetItem());
		m_pListWidgetShare->removeItemWidget(pThumb->GetItem());
		pThumb->SetItem(NULL);
		m_pListWidgetShare->takeItem(iRow);
		int iHeight = 174 * m_pListWidgetShare->count();
		m_pListWidgetShare->resize(222, iHeight);
		m_pListWidgetShare->update();
		QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(4));
		int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
		m_pListWidgetShare->resize(222, iHeight);
		m_pShareDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
		pThumb->deleteLater();
	}
	if (m_websiteThumbsMap.contains(key)) {
		pThumb = m_websiteThumbsMap.find(key).value();
		m_websiteThumbsMap.remove(key);
		disconnect(pThumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onListCloseWebEngineViewClicked(QString&, bool)));
		disconnect(pThumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onListOpenWebEngineViewClicked(QString&, bool)));

		int iRow = m_pListWidgetWebsite->row(pThumb->GetItem());
		m_pListWidgetWebsite->removeItemWidget(pThumb->GetItem());
		pThumb->SetItem(NULL);
		m_pListWidgetWebsite->takeItem(iRow);
		int iHeight = 174 * m_pListWidgetWebsite->count();
		m_pListWidgetWebsite->resize(222, iHeight);
		m_pListWidgetWebsite->update();
		QRect position = ui->listWidget->visualItemRect(ui->listWidget->item(8));
		int y = position.y() + ui->listWidget->geometry().y() + position.height() / 2 - iHeight / 2;
		m_pListWidgetWebsite->resize(222, iHeight);
		m_pWebsiteDockWidget->setGeometry(100, qMax<int>(0, y), 222, iHeight);
		pThumb->deleteLater();
	}
	axWidgetManage.closeWebEngineView(key);

}

void PowerdudeMain::onOneCloudWindowOpened(QString &key)
{
	if (oneCloudDataMap.contains(key)) {
		OneCloudData* pData = oneCloudDataMap[key];
		if (controlPanelInfoMap.contains(pData->getType())) {
			controlPanelInfo& cpInfo = controlPanelInfoMap[pData->getType()];
			if (cpInfo.pInfoListWidget != NULL) {
				cpInfo.pInfoListWidget->onOneCloudOpened(key);
				if (!ui->listWidget->item(cpInfo.index)->isSelected()) {
					ui->listWidget->item(cpInfo.index)->setSelected(true);
					ui->listWidget->item(cpInfo.index)->setData(Selected, true);
				}
			}
			else {
				ui->listWidget->item(cpInfo.index)->setSelected(true);
				ui->listWidget->item(cpInfo.index)->setData(Selected, true);
			}
		}
	}
}

void PowerdudeMain::onOneCloudWindowConnected(QString &key)
{
	if (oneCloudDataMap.contains(key)) {
		OneCloudData* pData = oneCloudDataMap[key];
		if (controlPanelInfoMap.contains(pData->getType())) {
			controlPanelInfo& cpInfo = controlPanelInfoMap[pData->getType()];
			if (cpInfo.pInfoListWidget != NULL) {
				cpInfo.pInfoListWidget->onOneCloudConnected(key);
			}
		}
	}
}

void PowerdudeMain::onOneCloudWindowDisconnected(QString &key)
{
	if (oneCloudDataMap.contains(key)) {
		OneCloudData* pData = oneCloudDataMap[key];
		if (controlPanelInfoMap.contains(pData->getType())) {
			controlPanelInfo& cpInfo = controlPanelInfoMap[pData->getType()];
			if (cpInfo.pInfoListWidget != NULL) {
				cpInfo.pInfoListWidget->onOneCloudDisconnected(key);
			}
		}
	}
}

void PowerdudeMain::onOneCloudWindowCloseed(QString &key)
{
	if (oneCloudDataMap.contains(key)) {
		OneCloudData* pData = oneCloudDataMap[key];
		if (controlPanelInfoMap.contains(pData->getType())) {
			bool isSelected = false;
			controlPanelInfo& cpInfo = controlPanelInfoMap[pData->getType()];
			if (cpInfo.pInfoListWidget != NULL) {
				cpInfo.pInfoListWidget->onOneCloudCloseed(key);
				if (!cpInfo.pInfoListWidget->isXBtnShow()) {
					ui->listWidget->item(cpInfo.index)->setSelected(false);
					ui->listWidget->item(cpInfo.index)->setData(Selected, false);
				}
			}
			else {
				ui->listWidget->item(cpInfo.index)->setSelected(false);
				ui->listWidget->item(cpInfo.index)->setData(Selected, false);
			}
		}
	}
}

void PowerdudeMain::onOneCloudWindowToConnect(QString &key)
{
	if (oneCloudDataMap.contains(key)) {
		OneCloudData* pData = oneCloudDataMap[key];
		if (controlPanelInfoMap.contains(pData->getType())) {
			controlPanelInfo& cpInfo = controlPanelInfoMap[pData->getType()];
			if (cpInfo.pInfoListWidget != NULL) {
				cpInfo.pInfoListWidget->onOneCloudToConnect(key);
			}
		}
	}
}

void PowerdudeMain::onOpenOneCloudWindow(QString &key)
{
	if (oneCloudManage.contains(key)) {
		oneCloudManage.openOneCloudMainWindow(key);
	}
	else {
		if (oneCloudDataMap.contains(key)) {
			OneCloudData* pData = oneCloudDataMap[key];
			OneCloudView* pView = OneCloudDataFactory::createOneCloudView(pData->getType(), pData);
			oneCloudManage.openOneCloudMainWindow(key, pData->getData("name").toString(), pView, controlPanelInfoMap[pData->getType()].icon);
		}
	}
}

void PowerdudeMain::onConnectOneCloudWindow(QString &key)
{
	oneCloudManage.connectHost(key);
}

void PowerdudeMain::onDisconnectOneCloudWindow(QString &key)
{
	oneCloudManage.disconnectHost(key);
}

void PowerdudeMain::onCloseOneCloudWindow(QString &key)
{
	oneCloudManage.closeOneCloudMainWindow(key);
}

void PowerdudeMain::onUpdateWidget()
{
	InfoListWidget * pInfoListWidget = qobject_cast<InfoListWidget *>(sender());
	if (webSocket->isLogin()) {
		if (pInfoListWidget == ui->remoteDesktopFrame) {
			pInfoListWidget->clearNotOpenInfoWidget();
			cloudblkmgr->getDesktopList(webSocket->m_chatSvrInfo, "/business/user/desktop/list");
		}else if (pInfoListWidget == ui->VNCFrame) {
			cloudblkmgr->getVncDesktopList(webSocket->m_chatSvrInfo, "/business/user/vnc/list");
		}
		else if (pInfoListWidget == ui->SSHFrame) {
			cloudblkmgr->getSSHDesktopList(webSocket->m_chatSvrInfo, "/business/user/ssh/list");
		}
		else if (pInfoListWidget == ui->shareDesktopFrame) {
			cloudblkmgr->getDesktopLiveList(webSocket->m_chatSvrInfo, "/business/user/desktop/play/list");
		}
		else if (pInfoListWidget == ui->webFrame) {
			cloudblkmgr->getWebsiteList(webSocket->m_chatSvrInfo, "/business/user/thirdparty/list");
		}
		else if (pInfoListWidget == ui->fileFrame) {
			//cloudblkmgr->getWebsiteList(webSocket->m_chatSvrInfo, "/business/user/thirdparty/list");
			cloudblkmgr->getDesktopFileList(webSocket->m_chatSvrInfo, "/business/user/desktop/file/list");
		}
        else if (pInfoListWidget == ui->rdpGroupFrame) {
            cloudblkmgr->getDesktopGroupList(webSocket->m_chatSvrInfo);
        }
	}
}

void PowerdudeMain::xorData(QByteArray &data)
{
	static QByteArray key1 = "myprivatekey";
	static QByteArray key2 = "helloworld";
	for (int i = 0; i < data.size(); ++i)
	{
		int keyIndex1 = i % key1.size();
		int keyIndex2 = i % key2.size();
		data[i] = data[i] ^ key1[keyIndex1];
		data[i] = data[i] ^ key2[keyIndex2];
	}
}

void PowerdudeMain::listViewInit(QString &str)
{
	for (auto iter = controlPanelInfoMap.begin(); iter != controlPanelInfoMap.end(); ++iter) {
		iter.value().index = -1;
	}
	QStringList list = str.split(",");
	ui->listWidget->clear();
	for (int i = 0;i < list.size();i++) {
		int type = list[i].toInt();
		controlPanelInfo& cpInfo = controlPanelInfoMap[type];
		ui->listWidget->addItem(cpInfo.name);
		ui->listWidget->item(i)->setData(Type, type);
		ui->listWidget->item(i)->setIcon(cpInfo.icon);
		cpInfo.index = i;
		//ui->listWidget->item(i)->setText(cpInfo.name);
	}
}

void PowerdudeMain::onGetVersion(QString & version)
{
	QString cur_version = qApp->applicationVersion();
	QString latest_ver = version;

	Update *pb_update = qobject_cast<Update *>(sender());
	bool isvalid = pb_update->check_updatefile(pb_update->file_path);

	SingleTon<Settings>::getInstance().beginGroup("Globa");
	QString jsonUrl = SingleTon<Settings>::getInstance().value("updateJson", DEFAULT_UPDATE_URL).toString();
	SingleTon<Settings>::getInstance().endGroup();

	if (pb_update->compare(latest_ver, cur_version) && isvalid) {
		QSimpleUpdater* m_updater = QSimpleUpdater::getInstance();
		m_updater->checkForUpdates(jsonUrl);
	}
	QObject::disconnect(pb_update, &Update::getVersion, this, &PowerdudeMain::onGetVersion);
}

void PowerdudeMain::onNetworkComplete(QString & key, QString & url)
{
	if (mstscRdpInfoMap.contains(key)) {
		MstscRdpInfo info = mstscRdpInfoMap[key];
		if (info.type == 6) {
			ui->SSHFrame->getInfoWidget(key)->setConnectStatus(InfoWidget::Connecting);
		}
		else if (info.type == 8) {
			ui->VNCFrame->getInfoWidget(key)->setConnectStatus(InfoWidget::Connecting);
		}
		else if (info.type == 4) {
			//ui->shareDesktopFrame->addInfoWidget(key, info.name, info.type, InfoWidget::Disconnecting)->hideX();
			ui->shareDesktopFrame->getInfoWidget(key)->setConnectStatus(InfoWidget::Connecting);
		}
		axWidgetManage.setUrl(key, url);
	}

}

void PowerdudeMain::onH5chatLogin(QString &url, ChatServerInfo &chatSvrInfo)
{
    ui->listWidget->item(0)->setSelected(true);
    ThumbWidget* thumb = axWidgetManage.openWebEngineView(ui->listWidget->item(0)->whatsThis(), url, ui->listWidget->item(0)->icon());
    if (thumb != 0) {
        DockWidget* pDockWidget = 0;
        if (m_dockMaps.contains(ui->listWidget->item(0)->whatsThis())) {
            pDockWidget = m_dockMaps.find(ui->listWidget->item(0)->whatsThis()).value();
        }
        if (pDockWidget == 0) {
            pDockWidget = new DockWidget(ui->widget_3);
            connect(thumb, SIGNAL(thumbCloseWidget(QString&, bool)), this, SLOT(onCloseWebEngineViewClicked(QString&, bool)));
            connect(thumb, SIGNAL(thumbOpenWidget(QString&, bool)), this, SLOT(onOpenWebEngineViewClicked(QString&, bool)));
            if (!m_dockMaps.contains(ui->listWidget->item(0)->whatsThis())) {
                m_dockMaps.insert(ui->listWidget->item(0)->whatsThis(), pDockWidget);
            }
        }
        pDockWidget->SetThumbWidget(thumb);
    }

    webSocket->setChatServerInfo(chatSvrInfo);
    if (!webSocket->m_isLogin) {
        webSocket->m_isLogin = true;
        //获取头像信息
        HttpClient(webSocket->m_chatSvrInfo.chatServerApi + "/business/user/get")
            .addParam("token", webSocket->m_chatSvrInfo.token)
            .addHeader("Oc_Platform_AppVersion", "3.0.0")
            .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
            .get([=](const QString &response) mutable {
            qDebug() << response;

            QJsonParseError jsonerror;
            QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

            if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
                QJsonObject jsonObject = document.object();
                int code = document.object().value("code").toInt();
                if (0 == code) {
                    QString url = jsonObject.value("avatar_url").toString();
                    QString name = jsonObject.value("nick_name").toString();
                    QString mobile = jsonObject.value("mobile").toString();
                    emit webSocket->userInfo(url, name, mobile);
                }
            }
        });
    }
    emit webSocket->login();
}