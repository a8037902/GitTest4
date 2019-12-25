#include "autohideframe.h"
#include <QPropertyAnimation>


AutoHideFrame::AutoHideFrame(QWidget* parent)
	:QFrame(parent)
	, m_size(0, 0),
	m_pDockWidget(0),
	m_pDockMenu(0),
	m_pToolMenu(0),
	m_pTimer(new QTimer(this)),
	isShow(0)
{
	//m_size = this->size();
	//m_size = this->rect().size();
	m_pTimer->setSingleShot(true);
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onHideFrame()));
}


AutoHideFrame::~AutoHideFrame()
{

}
bool AutoHideFrame::isAutoHide() const
{
	return autoHide;
}
void AutoHideFrame::setAutoHide(bool autoHide)
{
	this->autoHide = autoHide;
}

AutoHideFrame::Driection AutoHideFrame::getHideDriection() const
{
	return hideDriection;
}

void AutoHideFrame::setHideDriection(Driection hideDriection)
{
	this->hideDriection = hideDriection;
}
void AutoHideFrame::leaveEvent(QEvent *event)
{
	if (isShow == 1) {
		if (autoHide && (m_pDockWidget == 0 && m_pDockMenu == 0 && m_pToolMenu == 0))
		{
			emit aboutTohide();
			hideWidget();
		}
		else if (autoHide && (m_pDockWidget != 0 || m_pDockMenu != 0 || m_pToolMenu != 0)) {
			m_pTimer->stop();
			m_pTimer->start(300);
		}
	}
}

void AutoHideFrame::enterEvent(QEvent *event)
{
	if (autoHide && isShow == 0)
	{
		if (hideDriection == Centre) {
			m_pTimer->stop();
			showWidget();
		}
		else {
			emit waitToHide();
		}
	}
}

void AutoHideFrame::registerDockWidget(DockWidget* pDockWidget)
{
	if (pDockWidget != 0) {
		unRegisterDocketWidget();
		m_pDockWidget = pDockWidget;
		connect(m_pDockWidget, SIGNAL(toHideWidget()), this, SLOT(onHideFrame()));
		connect(m_pDockWidget, SIGNAL(toKeepWidget()), this, SLOT(onKeepFrame()));
		m_pDockWidget->show();
		m_pDockWidget->raise();
		m_pDockWidget->OnRegistered(true);
	}
}

void AutoHideFrame::unRegisterDocketWidget()
{
	if (m_pDockWidget != 0) {
		m_pDockWidget->OnRegistered(false);
		disconnect(m_pDockWidget, SIGNAL(toHideWidget()), this, SLOT(onHideFrame()));
		disconnect(m_pDockWidget, SIGNAL(toKeepWidget()), this, SLOT(onKeepFrame()));
		m_pDockWidget->hide();
		m_pDockWidget = 0;
	}
}

void AutoHideFrame::setDockMenu(DockMenu* pDockMenu)
{
	if (pDockMenu != 0) {
		m_pDockMenu = pDockMenu;
		connect(m_pDockMenu, SIGNAL(toHideWidget()), this, SLOT(onHideFrame()));
		connect(m_pDockMenu, SIGNAL(toKeepWidget()), this, SLOT(onKeepFrame()));
	}
}

void AutoHideFrame::setToolMenu(ToolMenu* pToolMenu)
{
	if (pToolMenu != 0) {
		m_pToolMenu = pToolMenu;
		connect(m_pToolMenu, SIGNAL(toHideWidget()), this, SLOT(onHideFrame()));
		connect(m_pToolMenu, SIGNAL(toKeepWidget()), this, SLOT(onKeepFrame()));
	}
}

//void AutoHideFrame::resizeEvent(QResizeEvent * event)
//{
//	m_size = this->size();
//}

void AutoHideFrame::ToHideWidget()
{
	m_pTimer->stop();
	showWidget();
}

void AutoHideFrame::hideWidget(bool isAnimate)
{
	isShow = 0;
	if (isAnimate) {
		if (hideDriection == Centre)
		{
			hide();
			return;
		}

		if (m_size.width() == 0 || m_size.height() == 0) {
			return;
			m_size = this->size();
		}
		QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
		animation->setDuration(100);
		animation->setStartValue(QRect(this->pos(), this->size()));
		QRect rcEnd;
		if (hideDriection == Up)
		{
			rcEnd = QRect(this->x(), -this->height() + 2, m_size.width(), m_size.height());
		}
		else if (hideDriection == Left)
		{
			rcEnd = QRect(-this->width() + 2, this->y(), m_size.width(), m_size.height());
		}
		else if (hideDriection == Right)
		{
			rcEnd = QRect(this->width() - 2, this->y(), m_size.width(), m_size.height());
		}
		animation->setEndValue(rcEnd);
		animation->start();
	}
	else {
		QRect rcEnd;
		if (hideDriection == Up)
		{
			rcEnd = QRect(this->x(), -this->height() + 2, m_size.width(), m_size.height());
		}
		else if (hideDriection == Left)
		{
			rcEnd = QRect(-this->width() + 2, this->y(), m_size.width(), m_size.height());
		}
		else if (hideDriection == Right)
		{
			rcEnd = QRect(this->width() - 2, this->y(), m_size.width(), m_size.height());
		}
		this->setGeometry(rcEnd);
	}
}

void AutoHideFrame::showWidget()
{
	isShow = 1;
	if (hideDriection == Centre)
	{
		show();
		return;
	}
	QPoint pos = this->pos();

	QPropertyAnimation *animation = new QPropertyAnimation(this, "geometry");
	animation->setDuration(100);
	animation->setStartValue(QRect(pos, this->size()));

	QRect rcEnd;
	if (hideDriection == Up)
	{
		rcEnd = QRect(this->x(), 0, m_size.width(), m_size.height());
	}
	else if (hideDriection == Left)
	{
		rcEnd = QRect(0, this->y(), m_size.width(), m_size.height());
	}
	else if (hideDriection == Right)
	{
		rcEnd = QRect(this->width() - 2, this->y(), m_size.width(), m_size.height());
	}
	animation->setEndValue(rcEnd);
	animation->start();
}

void AutoHideFrame::onHideFrame()
{
	unRegisterDocketWidget();
	if (m_pDockMenu != 0) {
		m_pDockMenu->hide();
	}
	if (m_pToolMenu != 0) {
		m_pToolMenu->hide();
	}
	emit aboutTohide();
	hideWidget();
}

void AutoHideFrame::onKeepFrame()
{
	m_pTimer->stop();
}