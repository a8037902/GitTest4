#include "DockMenu.h"

DockMenu::DockMenu(QWidget *parent)
	: QDialog(parent),
	m_pVBoxLayout(0)
{

	this->resize(100, 107);

	this->setContentsMargins(0, 0, 0, 0);

	QLabel* pLabel = new QLabel(this);
	pLabel->setGeometry(0, 0, this->size().width(), this->size().height());
	pLabel->setStyleSheet("background-image: url(:/powerdudemain/Resources/menu_bkimage.png);");

	setWindowFlags(Qt::FramelessWindowHint);

	QFont font;
	font.setFamily("微软雅黑");
	font.setPixelSize(12);
	this->setFont(font);
	//m_pVBoxLayout = new QVBoxLayout(this);
	//this->setLayout(m_pVBoxLayout);
	//m_pVBoxLayout->setContentsMargins(0, 0, 0, 0);
	//m_pVBoxLayout->setMargin(0);

	m_pBtnLock = new QPushButton(QStringLiteral("直接锁屏"), this);
	m_pBtnLock->setFlat(true);
	m_pBtnLock->setStyleSheet("color: white;text-align: left");
	m_pBtnLock->setGeometry(0, 0, this->size().width(), 35);
	//m_pVBoxLayout->addWidget(m_pBtnLock);
	m_pBtnLock->setContentsMargins(0, 0, 0, 0);
	//m_pBtnLock->setEnabled(false);

	QLabel* pLine = new QLabel(this);
	pLine->setGeometry(0, 35, this->size().width(), 1);
	pLine->setStyleSheet("background-color: rgb(160,160,160);");
	//m_pVBoxLayout->addWidget(pLine);
	pLine->setMargin(0);
	pLine->setContentsMargins(0, 0, 0, 0);

	m_pBtnMin = new QPushButton(QStringLiteral("最小化"), this);
	m_pBtnMin->setFlat(true);
	m_pBtnMin->setGeometry(0, 36, this->size().width(), 35);
	m_pBtnMin->setContentsMargins(0, 0, 0, 0);

	m_pBtnMin->setStyleSheet("color: white;text-align: left");

	//m_pVBoxLayout->addWidget(m_pBtnMin);


	pLine = new QLabel(this);
	pLine->setGeometry(0, 71, this->size().width(), 1);
	pLine->setStyleSheet("background-color: rgb(160,160,160);");
	//m_pVBoxLayout->addWidget(pLine);

	m_pBtnExit = new QPushButton(QStringLiteral("退出"), this);
	m_pBtnExit->setGeometry(0, 72, this->size().width(), 35);
	m_pBtnExit->setFlat(true);
	m_pBtnExit->setStyleSheet("color: white;text-align: left");
	//m_pVBoxLayout->addWidget(m_pBtnExit);
	m_pBtnExit->setContentsMargins(0, 0, 0, 0);

	this->setMouseTracking(true);
}

DockMenu::~DockMenu()
{

}

void DockMenu::leaveEvent(QEvent * event)
{
	emit toHideWidget();
}

void DockMenu::enterEvent(QEvent * event)
{
	emit toKeepWidget();
}