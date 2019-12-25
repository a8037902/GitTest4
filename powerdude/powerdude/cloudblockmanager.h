#pragma once
#include <QObject>
#include <QNetworkReply>
#include "chatsvrinfo.h"
#include "MstscRdpInfo.h"
#include "VncSshDesktop.h"
#include "logindatahandler.h"

class CloudBlockManager :public QObject
{
    Q_OBJECT
public:
    explicit CloudBlockManager();
    ~CloudBlockManager() {};

    /**
     * Get cloud desktop list
     */
    QList<MstscRdpInfo> getDesktopList(ChatServerInfo &info, QString url);

	void getDesktopFileList(ChatServerInfo &info, QList<MstscRdpInfo>&list, QList<int> listDesktopId, int index);
	QList<MstscRdpInfo> getDesktopFileList(ChatServerInfo &info, QString url);

    /**
     * Get VNC desktop list
     */
    QList<MstscRdpInfo> getVncDesktopList(ChatServerInfo &info, QString url);

    /**
     * Get SSH desktop list
     */
    QList<MstscRdpInfo> getSSHDesktopList(ChatServerInfo &info, QString url);

    /**
     * Get website list
     */
    void getWebsiteList(ChatServerInfo &info, QString url);

    /**
     * Get H5 chat login url
     */
    void getH5ChatLoginUrl(ChatServerInfo &info, QString url);

    /**
     * Get space chat login url
     */
    void getSpaceLoginUrl(ChatServerInfo &info, QString url);

    /**
     * Get live desktop list
     */
    QList<MstscRdpInfo> getDesktopLiveList(ChatServerInfo &info, QString url);

    /**
     * Construct single desktop live data which is the post requirement parameter of getting desktop live url
     */
    void constructSingleDesktopLiveData(ChatServerInfo &chatSvrInfo, QString &desktopLiveId, QString &key);

    /**
     * Get desktop group list
     */
    void getDesktopGroupList(ChatServerInfo &info);

    /**
     * Get desktop group info
     */
    void getDesktopGroupInfo(ChatServerInfo &info, QList<MstscRdpInfo> &list, QList<int> &desktopGroupIdList,
        QMap<int, QList<int>> &desktopGroupMap, int index);


    /**
     * Get desktop info
     */
    void getDesktopInfo(ChatServerInfo &info, QList<MstscRdpInfo> &list, 
        QMap<int, QList<int>> &desktopGroupMap, QMap<int, QList<int>>::const_iterator &i, int index, int desktopIndex);


signals:
    void desktopList(QList<MstscRdpInfo> &infoList);
    void textReceived(MstscRdpInfo &info, QString &url);
    void textReceived(QList<MstscRdpInfo>& list);
    void onDesktopLiveMsgChanged();

private slots:
    void onNetworkComplete(QNetworkReply *reply);

public:
    VncSshDesktop *m_vncSshDesktop;
};