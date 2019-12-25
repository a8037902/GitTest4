#pragma once
#include <QString>
#include <QSize>

struct WebsiteData {
    int bizId;
    QString thirdpartyName;
    QString accessUrl;
};

struct USBDesktopAuthData {
    QString bizId;
    QString serialCode;
    QString serialPwd;
};

enum RDPDestkopType {
    NormalDesktop = 1,
    P2PDesktop,
    DynamicCmdDesktop = 4,       // 动态口令桌面
    USBDesktop = 6,
    FaceAuthDesktop              // 刷脸桌面
};


const int DESKTOP_GROUP_TYPE = 100;
struct DesktopGroup{
    QString bizId;   // 桌面组业务ID
    QString name;    // 桌面组名称
    int count;       // 包含桌面数
};


class MstscRdpInfo
{
public:
	QString name;
	QString fulladdress;
	long port;
	QString username;
	QString password;
	QString domain;
	QString drivestoredirect;
	QSize desktopsize;
	int type;
    QString msg;
	QString filepath;
    RDPDestkopType desktopType;
    QString prekey;                  // 二次扫码上一次ws连接的key值
    QString xiaohui;                 // 二次扫码人的小慧号

    QString desktopLiveId;
    bool host;
    int hostStatus;
    QString subject;

    WebsiteData websitedata;          // website login data
    USBDesktopAuthData usbAuthData;   // USB auth data

    QString p2pId;                    // P2P destkop id

    DesktopGroup desktopGroupData;

    QList<QString> desktopKeyList;

	MstscRdpInfo() {}

	MstscRdpInfo& operator=(const MstscRdpInfo& info) {
		name = info.name;
		fulladdress = info.fulladdress;
		port = info.port;
		username = info.username;
		password = info.password;
		domain = info.domain;
		drivestoredirect = info.drivestoredirect;
		type = info.type;
		return *this;
	}

	bool operator==(const MstscRdpInfo& info) const {
		return fulladdress == info.fulladdress
			&& username == info.username;
	}

	bool operator!=(const MstscRdpInfo &info) const {
		return !(*this == info);
	}
};