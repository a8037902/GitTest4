#include "onecloudmainwindow.h"
#include "translucentbutton.h"
#include <qDebug>

namespace OneCloud {
	OneCloudMainWindow::OneCloudMainWindow(OneCloudView* pOneCloudView, QWidget *parent)
		: QWidget(parent)
	{

		QPalette palette;
		QPixmap bgImage(":/powerdudemain/Resources/u9.jpeg");
		bgImage.scaled(parent->size());//, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
		palette.setBrush(QPalette::Background, QBrush(bgImage.scaled(parent->size())));

		QWidget *pBack = new QWidget(this);
		pBack->setPalette(palette);
		pBack->setAutoFillBackground(true);

		pBack->setGeometry(0, 0, parent->width(), parent->height());

		this->pOneCloudView = pOneCloudView;
		pOneCloudView->init(this, QRect(QPoint(0, 0), parent->size()));

		QFont font("Microsoft YaHei", 16, 75);
		font.setPixelSize(20);

		pConnectPng = new QLabel(pBack);
		pConnectPng->setGeometry(parent->width() / 2 - 150, 200, 50, 50);
		pConnectPng->setPixmap(QPixmap(":/powerdudemain/Resources/connect.png"));
		pConnectPng->setScaledContents(true);
		pConnectPng->hide();

		pConnectText = new QLabel(pBack);
		pConnectText->setGeometry(parent->width() / 2 - 100, 200, 250, 50);
		pConnectText->setText(QStringLiteral("桌面正在连接中,请稍后..."));
		pConnectText->setFont(font);
		QPalette pe;
		pe.setColor(QPalette::WindowText, Qt::white);
		pConnectText->setPalette(pe);
		pConnectText->hide();

		pDisconnectingWidget = new QWidget(pBack);
		pDisconnectingWidget->setGeometry(0, 0, parent->width(), parent->height());
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

		connect(pOneCloudView, SIGNAL(connected()), this, SLOT(onConnected()));
		connect(pOneCloudView, SIGNAL(disconnected()), this, SLOT(onDisconnected()));

		pOneCloudView->hide();
	}


	OneCloudMainWindow::~OneCloudMainWindow()
	{

	}

	void OneCloudMainWindow::disconnectHost()
	{
		
		pOneCloudView->disconnectHost();
	}

	void OneCloudMainWindow::connectHost()
	{
		pConnectPng->show();
		pConnectText->show();
		pDisconnectingWidget->hide();
		emit toConnect(key);
		pOneCloudView->connectHost();
	}
	void OneCloudMainWindow::onConnected()
	{
		pConnectPng->hide();
		pConnectText->hide();
		pOneCloudView->show();
		emit connected(key);
	}
	void OneCloudMainWindow::onDisconnected()
	{
		pConnectPng->hide();
		pConnectText->hide();
		pDisconnectingWidget->show();
		pOneCloudView->hide();
		emit disconnected(key);
	}
	void OneCloudMainWindow::onConnectedBtn()
	{
		
		connectHost();
	}
}
