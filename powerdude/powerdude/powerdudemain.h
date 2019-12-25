#pragma once
#include <QWidget>
#include <QWebEngineView>
#include <QSettings>
#include "ui_powerdudemain.h"
#include "MstscRdpInfo.h"
#include "websocket.h"
#include "axwidgetmanage.h"
#include "InfoWidget.h"
#include "LoginWidget.h"
#include "setdialog.h"
#include "vncsshdesktop.h"
#include "InfoListWidget.h"
#include "cloudblockmanager.h"
//#include "ScreenRecFrame.h"
#include "DockMenu.h"
#include <qtimer.h>
#include "OneCloud\onecloudmanage.h"
#include "lock.h"
#include <TlHelp32.h>
#include "onecloudmessagebox.h"
#include "screenrecorddialog.h"
#include "tabicondialog.h"
#include "config.h"
#include "ThumbnailWidget.h"

//class SettingData
//{
//public:
//	int colorDepth;
//	bool redirectDrives;
//	bool redirectDevices;
//	bool redirectPrinters;
//	bool redirectPorts;
//};
using namespace OneCloud;

struct controlPanelInfo {
	InfoListWidget* pInfoListWidget;
	QIcon icon;
	QString name;
	QString key;
	int index;
};

class PowerdudeMain :
	public QWidget
{
	Q_OBJECT
public:
	explicit PowerdudeMain(QWidget *parent = Q_NULLPTR);
	~PowerdudeMain();

	enum ListViewItemData {
		Type = 101,
		Selected,
	};

	//连接远程系统
	void connectRemoteSystem();
	//取消连接远程系统
	void disconnectRemoteSystem();

public:
	void startHookRDPDisplay();
	ThumbWidget* getThumbWidget(HWND hWnd);

	void highLightUpdate();

signals:
	void aboutToClose();
    void onSecondScanFinished(int &result);
    //void networkComplete(QList<MstscRdpInfo&> list);

protected:
	//close current widget event
	virtual void closeEvent(QCloseEvent *event);
	virtual void changeEvent(QEvent *e);

	//bool nativeEvent(const QByteArray &eventType, void *message, long *result);

	//void setConnectInfo(MstscRdpInfo &mstscRdpInfo);
	QRect checkRect(int ax, int ay, int aw, int ah);

	void xorData(QByteArray &data);
	void listViewInit(QString &);

private slots:
	void singleclicked(QListWidgetItem* item);
	void singleclicked2(QListWidgetItem* item);
	void onValueChanged(int);
	void onAboutToHide();
	void onWaitToHide();
	void onHideFrame();
	void onUnLock();
	void on_addBtn_clicked();
	void on_sysBtn_clicked();
	void on_setBtn_clicked();
	void onToolsBtnClicked();
	void onScreenShotClicked();
	void onScreenRecClicked();
	void onTimerScreenShot();
	void on_lockBtn_clicked();
	void on_openLoginBtn_clicked();
	void on_downBtn_clicked();
	void on_upBtn_clicked();
	void on_lockExitBtn_clicked();
	void on_exitBtn_clicked();
	void onTextReceived(MstscRdpInfo &info, QString &msg);
	void onTextReceived(QList<MstscRdpInfo>&);
	void onOneCloudDataReceived(int type,QJsonObject &jsonObject);
	void onLogin();
	void onLocalLogin(MstscRdpInfo &);
	void onClose();
	void onUserInfo(QString &url, QString &name, QString &mobile);
	void onWebSocketRaiseError(int iErrorType, QString& errorMsg);

	//axwidget event
	void onLoginComplete(QString &key);
	void onDisconnected(QString &key);
	void onAuthenticationWarningDismissed();
	void onAuthenticationWarningDisplayed();
	void onLogonError(int error);
	void onNetworkStatusChanged(uint qualityLevel, int  bandwidth, int  rtt);

	void onCloseWebEngineViewClicked(QString& key, bool isAxWidget);
	void onOpenWebEngineViewClicked(QString& key, bool isAxWidget);

	void onListCloseWebEngineViewClicked(QString& key, bool isAxWidget);
	void onListOpenWebEngineViewClicked(QString& key, bool isAxWidget);

	void onTimerOut();
	void onLoginTimerOut();

	void onShortcut(const QString &key);
	void onAltAndTab();
	void onAltAndD();
	void onAltAndA();
	void onAltUp();
	void onAltAndDel();
	void onCtrlAndA();
	void onAltAndG();
	
	void onOpenAxWidget(QString&);
	void onConnectAxWidget();
	void onConnectAxWidget(QString&);
	void onCloseAxWidget(QString&);

	void onOpenWebEngineView(QString&);
	void onConnectWebEngineView(QString&);
	void onCloseWebEngineView(QString&);

	void onOneCloudWindowOpened(QString&);
	void onOneCloudWindowConnected(QString&);
	void onOneCloudWindowDisconnected(QString&);
	void onOneCloudWindowCloseed(QString&);
	void onOneCloudWindowToConnect(QString &);

	void onOpenOneCloudWindow(QString&);
	void onConnectOneCloudWindow(QString&);
	void onDisconnectOneCloudWindow(QString&);
	void onCloseOneCloudWindow(QString&);

	void onUpdateWidget();

	void onGetVersion(QString &version);

	void onNetworkComplete(QString &key, QString &url);
	void onItemEntered(QListWidgetItem * item);
	void OnClose(QString&);

	void getPictrueSlot(QNetworkReply * reply);
	QPixmap PixmapToRound(QPixmap &src, int radius);

	friend BOOL WINAPI BitBltProc(HDC hdc, int x, int y, int cx, int cy, HDC hdcSrc, int x1, int y1, DWORD rop);

    void onH5chatLogin(QString &url, ChatServerInfo &chatSvrInfo);

private:
	Ui::PowerdudeMain* ui;
	//bool isConnect;
	WebSocket* webSocket;
    WebSocket *secondScanWebSocket;
	VncSshDesktop *pbVncSshDesktop;
    CloudBlockManager *cloudblkmgr;
	//QSettings* settings;

	AxWidgetManage axWidgetManage;
	//int remoteDesktopNum;
	OneCloudManage oneCloudManage;

	/*QMap<QString, InfoWidget*> infoWidgetMap;
	QMap<QString, InfoWidget*> infoVNCWidgetMap;
	QMap<QString, InfoWidget*> infoSSHWidgetMap;
	QMap<QString, InfoWidget*> infoShareDesktopWidgetMap;*/

    QNetworkRequest request;

	//QMap<QString, InfoWidget*> infoWidgetMap;
	QList<InfoListWidget*> infoListWidgetList;
	QMap<QString, MstscRdpInfo> mstscRdpInfoMap;
	QMap<QString, MstscRdpInfo> mstscRdpInfoMap2;
	QMap<QString, QString> websitedataMap;
	QMap<QString, OneCloudData*> oneCloudDataMap;
	QMap<int, controlPanelInfo> controlPanelInfoMap;
	controlPanelInfo controlPanelInfos[10];

	QListWidget*    m_pListWidget;
	DockWidget*		m_pRdpsDockWidget;

	QListWidget*    m_pListWidgetSSH;
	DockWidget*     m_pSSHDockWidget;
	QMap<QString, ThumbWidget*> m_sshThumbsMap;

	QListWidget*    m_pListWidgetVNC;
	DockWidget*     m_pVNCDockWidget;
	QMap<QString, ThumbWidget*> m_vncThumbsMap;


	QListWidget*    m_pListWidgetShare;
	DockWidget*     m_pShareDockWidget;
	QMap<QString, ThumbWidget*> m_shareThumbsMap;

	QListWidget*    m_pListWidgetWebsite;
	DockWidget*     m_pWebsiteDockWidget;
	QMap<QString, ThumbWidget*> m_websiteThumbsMap;

	QMap<QString, DockWidget*> m_dockMaps;

	DockMenu*       m_pDockMenu;
	ToolMenu*		m_pToolMenu;
    LoginDataHandler *m_pLoginDataHandler;

	QTimer* m_pTimer;
	bool waitHide;
	UserInfo userInfo;
	ScreenRecordDialog* m_pScreenRecordDialog;
	TabIconDialog*		m_pTabIconDialog;
	SetDialog*			m_pSetDialog;
	QTimer*timer;
};

