#pragma once
#include <QWebSocket>
#include <QTimer>
#include <QUuid>
#include <qprocess.h>
#include <onecloudwidget.h>
#include <QNetworkReply.h>
#include "MstscRdpInfo.h"
#include "chatsvrinfo.h"
#include "logindatahandler.h"

struct RemoteControlData {
    QString companyName;        // ��websocekt��ȡ���Ĺ�˾����chatservername
    QString nearbyEnable;       // �����ý����ȡ���Ŀ�������������ֵ
    QString punchNeed;          // һֱ��Ϊfalse
    QString mac;                // д��ע����UUID
    QString aliases;            // �����ý����ȡ����С������
    QString userName;           // ��ȡ�����ص��Ե��û���
    QString hostName;           // ��ȡ�����ص��Ե���������
    QString longitude;          // ��websocket��ȡ���ľ���
    QString latitude;           // ��websocket��ȡ����ά��
    QString xiaohui;            // ��websocket��ȡ����С�ۺ�
};


class WebSocket
	:public QWebSocket
{
	Q_OBJECT
public:
    ChatServerInfo m_chatSvrInfo;
    void setChatServerInfo(ChatServerInfo chatSvrInfo) { m_chatSvrInfo = chatSvrInfo; }
    ChatServerInfo ChatServerInfo() const { return m_chatSvrInfo; }

    LoginDataHandler *m_loginDataHandler;

    bool m_isLogin;
    QString _uuid;

	explicit WebSocket();
	~WebSocket();

	void start();
	bool isLogin() {
		return m_isLogin;
	}

	static QString getRandomString(int length);

	void createUrlFromWebTime();

signals:
	void textReceived(MstscRdpInfo &info, QString &msg);
	void oneCloudDataReceived(int type, QJsonObject & jsonObject);
    //void textReceived(QString &msg);
	void showUrl(QString &url);
	void userInfo(QString &url, QString &name, QString &mobile);
	void login();
	void scan();
    void localLogin(MstscRdpInfo &info);
	void raiseError(int iErrorType,QString &errorMsg);

public slots:
	void onLoginComplete();
    
private slots:
	void onDisconnected();
	void onConnected();
	void onTextReceived(QString msg);
	void onError(QAbstractSocket::SocketError error);
	void onSslErrors(const QList<QSslError> &errors);
	void reconnect();
	void updataUrl();
	void requestFinished(QNetworkReply*);

private:
	QTimer * _ptimer;
	QTimer * qrTimer;
    
	QString website;
    int debug;
    
    QProcess m_process;
	
    QString m_pdChatHost;
    QString m_spaceHost;
    QString m_pdHost;

    QString mPowerdudeUUID;
    RemoteControlData mRemoteControlData;
};

