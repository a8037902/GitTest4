#include "DockWidget.h"

DockWidget::DockWidget(QWidget *parent)
	: QWidget(parent),
	m_pThumbWidget(0),
	m_pThumbList(0)
{
	this->setMouseTracking(true);
	this->setContentsMargins(0, 0, 0, 0);
}

DockWidget::~DockWidget()
{

}

void DockWidget::SetThumbWidget(ThumbWidget* pThumbWidget)
{
	m_pThumbWidget = pThumbWidget;
	m_pThumbWidget->setParent(this);
	this->resize(m_pThumbWidget->size());
	m_pThumbWidget->setGeometry(0, 0, this->size().width(), this->size().height());
}

void DockWidget::SetThumbListWidget(QListWidget* pListWidget)
{
	m_pThumbList = pListWidget;
	m_pThumbList->setParent(this);
	this->resize(m_pThumbList->size());
	m_pThumbList->setGeometry(0, 0, this->size().width(), this->size().height());
}

ThumbWidget* DockWidget::GetThumbWidget()
{
	return m_pThumbWidget;
}

void DockWidget::leaveEvent(QEvent * event)
{
	emit toHideWidget();
}

void DockWidget::enterEvent(QEvent * event)
{
	emit toKeepWidget();
}

void DockWidget::OnRegistered(bool bIsRegistered)
{
	if (bIsRegistered) {
		if (m_pThumbWidget != 0) {
			m_pThumbWidget->StartThumb();
		}
		else if (m_pThumbList != 0) {
			for (int i = 0; i < m_pThumbList->count(); i++) {
				QListWidgetItem* item = m_pThumbList->item(i);
				if (0 != item) {
					ThumbWidget* thumb = (ThumbWidget*)m_pThumbList->itemWidget(item);
					if (0 != thumb) {
						thumb->StartThumb();
					}
				}
			}
		}
	}
	else {
		if (m_pThumbWidget != 0) {
			m_pThumbWidget->StopThumb();
		}
		else if (m_pThumbList != 0) {
			for (int i = 0; i < m_pThumbList->count(); i++) {
				QListWidgetItem* item = m_pThumbList->item(i);
				if (0 != item) {
					ThumbWidget* thumb = (ThumbWidget*)m_pThumbList->itemWidget(item);
					if (0 != thumb) {
						thumb->StopThumb();
					}
				}
			}
		}
	}
}