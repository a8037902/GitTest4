#pragma once

#include <QDialog>
#include <QFrame>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

class ToolMenu : public QDialog
{
	Q_OBJECT

public:
	ToolMenu(QWidget *parent);
	~ToolMenu();

protected:
	void leaveEvent(QEvent * event)override;
	void enterEvent(QEvent * event)override;

signals:
	void toHideWidget();
	void toKeepWidget();

public:
	QPushButton* m_pBtnScreenShot;
	QPushButton* m_pBtnScreenRec;
};
