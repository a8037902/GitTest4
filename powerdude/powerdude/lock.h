#pragma once
#include <qwidget.h>
#include <QKeyEvent>
#include <qcheckbox.h>
#include "ui_lock.h"

class UserInfo
{
public:
	UserInfo() {
		pixmap = NULL;
	}
	QString url;
	QString name;
	QString mobile;
	QPixmap *pixmap;
};
class Lock :
	public QWidget
{
	Q_OBJECT
public:
	Lock(QWidget *parent = Q_NULLPTR);
	~Lock();
	void setUserInfo(UserInfo&);
	void setChatServerApi(QString& api,QString &token);

signals:
	void unLock();
	void minClicked();
	void exitClicked();

private slots:
	void on_loginBtn_clicked();
	void on_minBtn_clicked();
	void on_exitBtn_clicked();
	void on_sysBtn_clicked();
	void timerUpdate();

protected:
	virtual void keyReleaseEvent(QKeyEvent *event);

private:
	Ui::Lock* ui;

	QString chatServerApi;
	QString token;
};

