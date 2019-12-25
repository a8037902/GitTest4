#include "onecloudwidget.h"
#include <qDebug>
#include <QAxObject>
#include "translucentbutton.h"
/*dengpy*/
#include <rdpappcontainerclient.h>
#include <qScreen>
#include <qWindow>
#include <qpainter.h>
/*dengpy*/

OneCloudWidget::OneCloudWidget(int iDisplayWidth, int iDisplayHeight, QWidget *parent)
	: QWidget(parent), pAxWidget(NULL),hAxWnd(0), pThumbWidget(0)
	, m_iDisplayWidth(iDisplayWidth),m_iDisplayHeight(iDisplayHeight)
{
	QPalette palette;
	QPixmap bgImage(":/powerdudemain/Resources/u9.jpeg");
	bgImage.scaled(parent->size());//, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
	palette.setBrush(QPalette::Background, QBrush(bgImage.scaled(parent->size())));
	this->setPalette(palette);
	this->setAutoFillBackground(true);

	this->setGeometry(0, 0, parent->width(),parent->height());

	QFont font("Microsoft YaHei", 16, 75);
	font.setPixelSize(20);

	pConnectPng = new QLabel(this);
	pConnectPng->setGeometry(parent->width() / 2 - 150, 200, 50, 50);
	pConnectPng->setPixmap(QPixmap(":/powerdudemain/Resources/connect.png"));
	pConnectPng->setScaledContents(true);
	pConnectPng->hide();

	pConnectText = new QLabel(this);
	pConnectText->setGeometry(parent->width() / 2 - 100, 200, 250, 50);
	pConnectText->setText(QStringLiteral("桌面正在连接中,请稍后..."));
	pConnectText->setFont(font);
	QPalette pe;
	pe.setColor(QPalette::WindowText, Qt::white);
	pConnectText->setPalette(pe);
	pConnectText->hide();

	pDisconnectingWidget = new QWidget(this);
	pDisconnectingWidget->setGeometry(0,0,parent->width(), parent->height());

	pDisconnectingWidget->hide();

	QLabel * pLabel = new QLabel(pDisconnectingWidget);
	pLabel->setGeometry(parent->width() / 2 - 300, 480, 100, 100);
	pLabel->setPixmap(QPixmap(":/powerdudemain/Resources/disconnect.png"));
	pLabel->setScaledContents(true);
	pLabel = new QLabel(pDisconnectingWidget);
	pLabel->setGeometry(parent->width() / 2 - 200, 480, 180, 50);
	pLabel->setText(QStringLiteral("桌面连接已经断开"));
	pLabel->setFont(font);
	pLabel->setPalette(pe);
	pLabel = new QLabel(pDisconnectingWidget);
	pLabel->setGeometry(parent->width() / 2 - 200, 500, 350, 50);
	pLabel->setText(QStringLiteral("请检查网络环境和桌面状态是否正常."));
	pLabel->setFont(font);
	pe.setColor(QPalette::WindowText, Qt::gray);
	pLabel->setPalette(pe);

	TranslucentButton *pBtn = new TranslucentButton(pDisconnectingWidget);
	pBtn->setGeometry(parent->width() / 2 - 200, 550, 100, 36);
	pBtn->setText(QStringLiteral("重新连接"));
	pBtn->setIcon(QIcon(":/powerdudemain/Resources/connect.png"));
	connect(pBtn, SIGNAL(clicked()), this, SLOT(onConnectedBtn()));

	pAxWidget = new QAxWidget(this);
	pAxWidget->setControl("{54d38bf7-b1ef-4479-9674-1bd6ea465258}");
	//pAxWidget->setWindowFlags(Qt::FramelessWindowHint);
	//pAxWidget->setStyleSheet("QGraphicsView{border:0px;}");
	//pAxWidget->setAutoFillBackground(false);
	

	//pAxWidget->setProperty("geometry", QVariant(QRect(QPoint(0, 0), parent->size())));
	//pAxWidget->setProperty("geometry", QVariant(QRect(QPoint(0, 0), QSize(1280,800))));
	pAxWidget->setProperty("geometry", QVariant(QRect(QPoint(0, 0), QSize(m_iDisplayWidth, m_iDisplayHeight))));

	pAxWidget->raise();
	pAxWidget->hide();

	connect(pAxWidget, SIGNAL(OnConnected()), this, SLOT(onConnected()));
	connect(pAxWidget, SIGNAL(OnConnecting()), this, SLOT(onConnecting()));
	connect(pAxWidget, SIGNAL(OnServiceMessageReceived(const QString&)), this, SLOT(onServiceMessageReceived(const QString&)));
	connect(pAxWidget, SIGNAL(OnWarning(int)), this, SLOT(onWarning(int)));

	connect(pAxWidget, SIGNAL(OnLoginComplete()), this, SLOT(onLoginComplete()));
	connect(pAxWidget, SIGNAL(OnDisconnected(int)), this, SLOT(onDisconnected(int)));
	connect(pAxWidget, SIGNAL(OnAuthenticationWarningDismissed()), this, SLOT(onAuthenticationWarningDismissed()));
	connect(pAxWidget, SIGNAL(OnAuthenticationWarningDisplayed()), this, SLOT(onAuthenticationWarningDisplayed()));
	connect(pAxWidget, SIGNAL(OnLogonError(int)), this, SLOT(onLogonError(int)));
	connect(pAxWidget, SIGNAL(OnNetworkStatusChanged(uint, int, int)), this, SLOT(onNetworkStatusChanged(uint, int, int)));

}


OneCloudWidget::~OneCloudWidget()
{

}

BOOL OneCloudWidget::EnumAxWidgetChildProc(HWND hWnd, LPARAM lParam)
{
	OneCloudWidget* pThis = reinterpret_cast<OneCloudWidget*>(lParam);
	wchar_t strBuffer[MAX_PATH] = { 0 };
	::GetWindowTextW(hWnd, strBuffer, MAX_PATH);
	if (lstrcmpW(strBuffer, L"Output Painter Child Window") == 0) {
		pThis->hAxWnd = hWnd;
	}
	return TRUE;
}

void OneCloudWidget::Connect()
{
	pConnectPng->show();
	pConnectText->show();
	pDisconnectingWidget->hide();
	pAxWidget->dynamicCall("Connect()");
}

void OneCloudWidget::Disconnect()
{
	pAxWidget->dynamicCall("Disconnect()");
}

/*dengpy*/
HWND OneCloudWidget::GetAxHwnd()
{
	::EnumChildWindows(reinterpret_cast<HWND>(pAxWidget->internalWinId()), EnumAxWidgetChildProc, reinterpret_cast<LPARAM>(this));
	return hAxWnd;
}

/*dengpy*/

void OneCloudWidget::setFilePath(QString &path) {
	filepath = path;
	//RemoteProgram2测试
	pRemoteProgram = pAxWidget->querySubObject("RemoteProgram2");
	if (pRemoteProgram) {
		pRemoteProgram->dynamicCall("SetRemoteProgramMode(bool)", true);
	}
}

void OneCloudWidget::closeEvent(QCloseEvent *event) {
	QWidget::closeEvent(event);

	disconnect(pAxWidget, SIGNAL(OnConnected()), this, SLOT(onConnected()));
	disconnect(pAxWidget, SIGNAL(OnConnecting()), this, SLOT(onConnecting()));
	disconnect(pAxWidget, SIGNAL(OnServiceMessageReceived(const QString&)), this, SLOT(onServiceMessageReceived(const QString&)));
	disconnect(pAxWidget, SIGNAL(OnWarning(int)), this, SLOT(onWarning(int)));

	disconnect(pAxWidget, SIGNAL(OnLoginComplete()), this, SLOT(onLoginComplete()));
	disconnect(pAxWidget, SIGNAL(OnDisconnected(int)), this, SLOT(onDisconnected(int)));
	disconnect(pAxWidget, SIGNAL(OnAuthenticationWarningDismissed()), this, SLOT(onAuthenticationWarningDismissed()));
	disconnect(pAxWidget, SIGNAL(OnAuthenticationWarningDisplayed()), this, SLOT(onAuthenticationWarningDisplayed()));
	disconnect(pAxWidget, SIGNAL(OnLogonError(int)), this, SLOT(onLogonError(int)));
	disconnect(pAxWidget, SIGNAL(OnNetworkStatusChanged(uint, int, int)), this, SLOT(onNetworkStatusChanged(uint, int, int)));
}

void OneCloudWidget::onConnected()
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	pAxWidget->show();
	//this->setGeometry(0, 112, 1280, 800);
	this->setGeometry(0.5*(this->parentWidget()->size().width() - m_iDisplayWidth), 0.5*(this->parentWidget()->size().height() - m_iDisplayHeight), m_iDisplayWidth, m_iDisplayHeight);

	emit OnLoginComplete();
}

void OneCloudWidget::onConnecting()
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
}

void OneCloudWidget::onServiceMessageReceived(const QString & serviceMessage)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
}

void OneCloudWidget::onWarning(int warningCode)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
}

void OneCloudWidget::onLoginComplete()
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	//pAxWidget->show();
	//emit OnLoginComplete();

	if (!filepath.isEmpty()) {
		pRemoteProgram->dynamicCall("ServerStartProgram(const QString&, const QString&, const QString&,bool, const QString&,bool)",
			filepath, QString(""), QString(""), FALSE, QString(""), FALSE);
			//QString("C:\\Program Files\\Mozilla Firefox\\firefox.exe"), QString(""), QString("C:\\Program Files\\Mozilla Firefox"), FALSE, QString(""), FALSE);
	}
}

void OneCloudWidget::onDisconnected(int discReason)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	pAxWidget->hide();
	pConnectPng->hide();
	pConnectText->hide();
	pDisconnectingWidget->show();
	this->setGeometry(0, 0, this->parentWidget()->size().width(), this->parentWidget()->size().height());
	emit OnDisconnected(discReason);
}

void OneCloudWidget::onAuthenticationWarningDismissed()
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	emit OnAuthenticationWarningDismissed();

	//pAxWidget->show();
	//emit OnLoginComplete();
}

void OneCloudWidget::onAuthenticationWarningDisplayed()
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	emit OnAuthenticationWarningDisplayed();
}

void OneCloudWidget::onLogonError(int error)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	emit OnLogonError(error);
}

void OneCloudWidget::onNetworkStatusChanged(uint qualityLevel, int bandwidth, int rtt)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	emit OnNetworkStatusChanged(qualityLevel, bandwidth, rtt);
}

void OneCloudWidget::onConnectedBtn()
{
	//Connect();
	emit toConnect(whatsThis());
}
