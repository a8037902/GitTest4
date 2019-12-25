#pragma once

#include <QWidget>
#include <QListWidget>
#include "ThumbWidget.h"

class DockWidget : public QWidget
{
	Q_OBJECT

public:
	DockWidget(QWidget *parent = nullptr);
	~DockWidget();

	void SetThumbWidget(ThumbWidget* pThumbWidget);
	ThumbWidget* GetThumbWidget();

	void SetThumbListWidget(QListWidget* pListWidget);

protected:
	void leaveEvent(QEvent * event)override;
	void enterEvent(QEvent * event)override;

signals:
	void toHideWidget();
	void toKeepWidget();

public:
	void OnRegistered(bool bIsRegistered);

private:
	ThumbWidget* m_pThumbWidget;
	QListWidget* m_pThumbList;
};
