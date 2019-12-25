#pragma once

#include <QWidget>
#include <QFrame>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <qdialog.h>

class DockMenu : public QDialog
{
	Q_OBJECT

public:
	DockMenu(QWidget *parent);
	~DockMenu();

protected:
	void leaveEvent(QEvent * event)override;
	void enterEvent(QEvent * event)override;

signals:
	void toHideWidget();
	void toKeepWidget();

public:
	QVBoxLayout* m_pVBoxLayout;
	QPushButton* m_pBtnExit;
	QPushButton* m_pBtnMin;
	QPushButton* m_pBtnLock;
};
