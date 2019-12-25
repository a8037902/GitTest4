#pragma once
#include <qwidget.h>
#include <QAxWidget>
#include <QLabel>
#include <QTimer>
#include "ThumbWidget.h"

class OneCloudWidget :
	public QWidget
{
	Q_OBJECT
public:
	explicit OneCloudWidget(int iDisplayWidth,int iDisplayHeight,QWidget* parent = nullptr);
	~OneCloudWidget();

	QAxWidget * getAxWidget()
	{
		return pAxWidget;
	}

	QWidget* getThumbWidget() {
		return pThumbWidget;
	}

	void setThumbWidget(QWidget* pThumb) {
		pThumbWidget = pThumb;
	}

	void Connect();
	void Disconnect();
	HWND GetAxHwnd();

	void setFilePath(QString &path);

signals:
	void OnLoginComplete();
	void OnDisconnected(int);
	void OnAuthenticationWarningDismissed();
	void OnAuthenticationWarningDisplayed();
	void OnLogonError(int);
	void OnNetworkStatusChanged(uint, int, int);

	void toConnect(QString&);

protected:
	virtual void closeEvent(QCloseEvent *event);

private slots:
	//axwidget event
	void onConnected();
	void onConnecting();
	void onServiceMessageReceived(const QString& serviceMessage);
	void onWarning(int warningCode);
	void onLoginComplete();
	void onDisconnected(int discReason);
	void onAuthenticationWarningDismissed();
	void onAuthenticationWarningDisplayed();
	void onLogonError(int error);
	void onNetworkStatusChanged(uint qualityLevel, int  bandwidth, int  rtt);
	void onConnectedBtn();
private:
	QAxWidget*pAxWidget;
	QLabel*pConnectPng;
	QLabel*pConnectText;
	QWidget *pDisconnectingWidget;
	QWidget *pThumbWidget;
	QAxObject *pRemoteProgram;
	/*dengpy*/
private:
	HWND  hAxWnd;
	static BOOL CALLBACK EnumAxWidgetChildProc(HWND hWnd, LPARAM lParam);
	/*dengpy*/
	QString filepath;
	int m_iDisplayWidth;
	int m_iDisplayHeight;
};

