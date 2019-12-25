#pragma once
#include <QObject>
#include <QMap>
#include <QProgressDialog>
#include "MstscRdpInfo.h"


class LoginDataHandler :public QObject
{
    Q_OBJECT
public:
    explicit LoginDataHandler();
    ~LoginDataHandler() {};

    QString hostxiaohui;    // ����ɨ�������С�ۺ�

    /**
     * Get desktop live url
     */
    void getDesktopLiveUrl(QString &url, QString &msg, MstscRdpInfo &info);

    /**
     * Get P2P destkop ip and port
     */
    void getP2PDesktopIpPort(MstscRdpInfo &info);

    /**
     * Receive file from appcc server
     */
    void downloadFile(MstscRdpInfo &info, QString &filename, QString &url);

    /**
     * Get space login url
     */
    void getSpaceLoginUrl(QString &msg);

    /**
     * USB auth
     */
    bool usbAuth(MstscRdpInfo &authData);

    /**
     * Desktop group data handle
     */
    void handleDesktopGroupData(QString &msg);

signals:
    void textReceived(MstscRdpInfo &info, QString &msg);

private:
    enum  P2PMode
    {
        P2P_SMART_MODE,           //P2P����ģʽ
        P2P_VPN_MODE,             //P2P���ģʽ
        P2P_RELAY_MODE            //P2P�м�ģʽ
    };
    QMap<P2PMode, QString> m_p2pMap;

    QProgressDialog *m_pProgressDlg;

    int detectDongle();

    bool dongleAuth(MstscRdpInfo &authData, int nCount);

    bool isMsgBoxClosed;
};