#include "ToolMenu.h"

ToolMenu::ToolMenu(QWidget *parent)
	: QDialog(parent),
	m_pBtnScreenShot(0)
{
	this->resize(100, 71);
	this->setContentsMargins(0, 0, 0, 0);
	QLabel* pLabel = new QLabel(this);
	pLabel->setGeometry(0, 0, this->size().width(), this->size().height());
	pLabel->setStyleSheet("background-image: url(:/powerdudemain/Resources/menu_bkimage.png);");

	setWindowFlags(Qt::FramelessWindowHint);

	QFont font;
	font.setFamily("Î¢ÈíÑÅºÚ");
	font.setPixelSize(12);
	this->setFont(font);

	m_pBtnScreenShot = new QPushButton(QStringLiteral("½ØÍ¼"), this);
	m_pBtnScreenShot->setFlat(true);
	m_pBtnScreenShot->setStyleSheet("color: white;text-align: left");
	m_pBtnScreenShot->setGeometry(0, 0, this->size().width(), 35);
	m_pBtnScreenShot->setContentsMargins(0, 0, 0, 0);

	QLabel* pLine = new QLabel(this);
	pLine->setGeometry(0, 35, this->size().width(), 1);
	pLine->setStyleSheet("background-color: rgb(160,160,160);");
	//m_pVBoxLayout->addWidget(pLine);
	pLine->setMargin(0);
	pLine->setContentsMargins(0, 0, 0, 0);

	m_pBtnScreenRec = new QPushButton(QStringLiteral("Â¼ÆÁ"), this);

	m_pBtnScreenRec->setFlat(true);
	m_pBtnScreenRec->setStyleSheet("color: white;text-align: left");
	m_pBtnScreenRec->setGeometry(0, 36, this->size().width(), 35);
	m_pBtnScreenRec->setContentsMargins(0, 0, 0, 0);

}

ToolMenu::~ToolMenu()
{

}


void ToolMenu::leaveEvent(QEvent * event)
{
	emit toHideWidget();
}

void ToolMenu::enterEvent(QEvent * event)
{
	emit toKeepWidget();
}