#include "websocket.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QApplication.h>
#include <QNetworkRequest.h>
#include <QNetworkAccessManager.h>
#include <QNetworkReply.h>
#include <QHostInfo>
#include <QMessagebox>
#include "OneCloudMessageBox.h"
#include "settings.h"
#include "httpclient.h"

WebSocket::WebSocket():QWebSocket()
	,_ptimer(new QTimer())
	, qrTimer(new QTimer())
	, m_isLogin(false)
    , m_spaceHost("")
    , m_loginDataHandler(new LoginDataHandler())
{
	connect(this, SIGNAL(disconnected()), this, SLOT(onDisconnected()), Qt::AutoConnection);
	connect(this, SIGNAL(textMessageReceived(QString)), this, SLOT(onTextReceived(QString)), Qt::AutoConnection);
	connect(this, SIGNAL(connected()), this, SLOT(onConnected()), Qt::AutoConnection);
	connect(this, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(onError(QAbstractSocket::SocketError)));
	connect(this, SIGNAL(sslErrors(const QList<QSslError> &)), this, SLOT(onSslErrors(const QList<QSslError> &)));
	//connect(_ptimer, SIGNAL(timeout()), this, SLOT(reconnect()), Qt::AutoConnection);
	connect(qrTimer, SIGNAL(timeout()), this, SLOT(updataUrl()), Qt::AutoConnection);
	
	SingleTon<Settings>::getInstance().beginGroup("Globa");
    debug = SingleTon<Settings>::getInstance().value("debug", 0).toInt();
    if (debug)
    {
        QSslConfiguration config = this->sslConfiguration();
        config.setPeerVerifyMode(QSslSocket::VerifyNone);
        config.setProtocol(QSsl::TlsV1SslV3);
        this->setSslConfiguration(config);
    }
	website = SingleTon<Settings>::getInstance().value("website", "appcc.pispower.com").toString();
    
    QSettings settings("HKEY_CURRENT_USER\\Software\\onecloud", QSettings::NativeFormat);
    mRemoteControlData.mac = settings.value("POWERDUDEUUID", "").toString();//读
    if (mPowerdudeUUID.isEmpty()) {
        mRemoteControlData.mac = "UUID" + getRandomString(8);

        settings.setValue("POWERDUDEUUID", mRemoteControlData.mac);//写
    }

	SingleTon<Settings>::getInstance().endGroup();

    SingleTon<Settings>::getInstance().beginGroup("RemoteSense");  
    mRemoteControlData.aliases = SingleTon<Settings>::getInstance().value("name").toString();
    mRemoteControlData.nearbyEnable = SingleTon<Settings>::getInstance().value("searchNearby", "false").toString();
    mRemoteControlData.punchNeed = "false";
    SingleTon<Settings>::getInstance().endGroup();

    // 读取小慧名字，如果为空，则取值机器名称，如果机器名称为空，则取值“小智-<随机八位数>”， 并保存到设置里
    if (mRemoteControlData.aliases.isEmpty()) {
        mRemoteControlData.aliases = QHostInfo::localHostName();
        if (mRemoteControlData.aliases.isEmpty()) {
            mRemoteControlData.aliases = "小智" + getRandomString(8);
        }

        SingleTon<Settings>::getInstance().setOneValue("RemoteSense", "name", mRemoteControlData.aliases);
    }

    mRemoteControlData.userName = QDir::home().dirName();
    if (mRemoteControlData.userName.isEmpty()) {
        mRemoteControlData.userName = "default";
    }
    mRemoteControlData.hostName = QHostInfo::localHostName();
    if (mRemoteControlData.hostName.isEmpty()) {
        mRemoteControlData.hostName = "default";
    }
}


WebSocket::~WebSocket()
{
}

void WebSocket::start()
{
	m_isLogin = false;
	_uuid = getRandomString(8) + "_" + mRemoteControlData.mac;
	this->open(QUrl(QString("wss://%1/connection/wait?key=%2").arg(website,_uuid)));
}

//断开连接会触发这个槽函数
void WebSocket::onDisconnected()
{
#ifdef _DEBUG
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
#endif
	//_ptimer->start(3000);
	qrTimer->stop();
	//setPixmap();
	//emit disconnected();
}
//连接成功会触发这个槽函数
void WebSocket::onConnected()
{
	//_ptimer->stop();
	createUrlFromWebTime();

	//qrTimer->start(50000);
	//QWebSocket *pClient = qobject_cast<QWebSocket *>(sender());

	//emit showUrl(QString("https://%1/s?T=10&k=%2&t=%3").arg(website,_uuid, QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch())));
}
//收到服务发来的消息会触发这个槽函数
void WebSocket::onTextReceived(QString msg)
{
#ifdef _DEBUG
	qDebug() << msg;
#endif

	QJsonParseError error;
	QJsonDocument jsonDocument = QJsonDocument::fromJson(msg.toUtf8().data(), &error);
    QString token, tokenEncode, chatServerApi, chatServerApiEncode, openfireBosh, openfireBoshEncode,
        chatServerId, chatServerIdEncode, companyName, companyNameEncode, chatletCache, chatletCacheEncode,
        urlHost, url, xiaohui, xiaohuiEncode, nickname, nicknameEncode, avatar, avatarEncode, desktopLiveUrl,
        baseUrl, auth, filename;

    SingleTon<Settings>::getInstance().beginGroup("Globa");
    m_pdChatHost = SingleTon<Settings>::getInstance().value("pdchat_host", "https://pdchat.pispower.com").toString();
    m_pdHost = SingleTon<Settings>::getInstance().value("pd_host", "https://pd.pispower.com").toString();
    SingleTon<Settings>::getInstance().endGroup();

	if (jsonDocument.isNull()) {
		qDebug() << error.errorString();
		qDebug() << "===> please check the string " << msg.toUtf8().data();
		return;
	}
	QJsonObject jsonObject = jsonDocument.object();

	if (jsonObject.value("action").toString() == "scan") {
		
		//qrTimer->stop();
		emit scan();
		return;
	}

	if (jsonObject.value("action").toString() == "login") {
		if (!m_isLogin) {
			m_chatSvrInfo.chatServerApi = jsonObject.value("chatserverapi").toString();
			m_chatSvrInfo.chatServerId = jsonObject.value("chatserverid").toString();
			m_chatSvrInfo.chatServerName = jsonObject.value("chatservername").toString();
			m_chatSvrInfo.token = jsonObject.value("token7").toString();
			mRemoteControlData.companyName = m_chatSvrInfo.chatServerName;
		}

		QJsonObject jsonObjectData = jsonObject.value("data").toObject();
		MstscRdpInfo info;

		if (!m_isLogin) {
			m_chatSvrInfo.xiaohui = jsonObjectData.value("xiaohui").toString();
		}
		else {
			if (m_chatSvrInfo.xiaohui.compare(jsonObjectData.value("xiaohui").toString()) == 0) {
				emit raiseError(-1,QStringLiteral("您已登录该小智，不可重复扫码！"));
				return;
			}
			else {
				if (!OneCloudMessageBox::information(NULL, QStringLiteral("提示"), QStringLiteral("该设备已被其他人登录，您是否要以无状态方式登录。"))) {
					return;
				}
				//emit raiseError(-2, QStringLiteral("该设备已被其他人登录！"));
				//return;
			}
		}

		int type = jsonObject.value("type").toInt();

        QJsonArray jsonArrayData = jsonObject.value("data").toArray();
        if (!jsonArrayData.isEmpty()) {
            m_loginDataHandler->handleDesktopGroupData(msg);
        }
		
		
        info.desktopType = RDPDestkopType(jsonObjectData.value("type").toString().toInt());

        switch (type) {
        case 1:
            info.type = 11;
            info.websitedata.bizId = jsonObjectData.value("buzid").toString().toInt();
            info.websitedata.thirdpartyName = jsonObjectData.value("thirdpartyname").toString();

            baseUrl = jsonObjectData.value("url").toString();
            auth = jsonObjectData.value("auth").toString();
            url = QString("%1?auth=%2").arg(baseUrl).arg(auth);
            info.websitedata.accessUrl = url;

            emit textReceived(info, url);
            break;

        case 2:
			if (!m_isLogin) {
				mRemoteControlData.longitude = jsonObjectData.value("longitude").toString();
				mRemoteControlData.latitude = jsonObjectData.value("latitude").toString();
				mRemoteControlData.xiaohui = jsonObjectData.value("xiaohui").toString();
			}

            switch (RDPDestkopType(info.desktopType)) {
            case RDPDestkopType::NormalDesktop:  // Normal desktop type
                info.type = jsonObjectData.value("type").toString().toInt();
				if (m_isLogin) {
					info.type = 0;
				}
                info.name = jsonObjectData.value("name").toString();
                info.fulladdress = jsonObjectData.value("ip").toString();
                info.port = jsonObjectData.value("port").toString().toInt();
                info.username = jsonObjectData.value("username").toString();
                info.password = jsonObjectData.value("password").toString();
                info.domain = jsonObjectData.value("domain").toString();
                m_loginDataHandler->hostxiaohui = jsonObjectData.value("hostxiaohui").toString();

                emit textReceived(info, msg);
                break;

            case RDPDestkopType::P2PDesktop:  // P2P desktop type
                info.type = type;
				if (m_isLogin) {
					info.type = 0;
				}

                info.p2pId = jsonObjectData.value("p2pid").toString();
                info.type = jsonObjectData.value("type").toString().toInt();
                info.name = jsonObjectData.value("name").toString();
                info.username = jsonObjectData.value("username").toString();
                info.password = jsonObjectData.value("password").toString();
                info.domain = jsonObjectData.value("domain").toString();

                emit textReceived(info, msg);
                break;

            case RDPDestkopType::USBDesktop:
                info.type = type;
				if (m_isLogin) {
					info.type = 0;
				}
                info.name = jsonObjectData.value("name").toString();
                info.fulladdress = jsonObjectData.value("ip").toString();
                info.port = jsonObjectData.value("port").toString().toInt();
                info.username = jsonObjectData.value("username").toString();
                info.password = jsonObjectData.value("password").toString();
                info.domain = jsonObjectData.value("domain").toString();

                info.usbAuthData.bizId = jsonObjectData.value("bizid").toString();
                info.usbAuthData.serialCode = jsonObjectData.value("serialcode").toString();
                info.usbAuthData.serialPwd = jsonObjectData.value("serialpwd").toString();

                emit textReceived(info, msg);
                break;

            case RDPDestkopType::FaceAuthDesktop:
                info.type = 0;    // 手动修改为无状态登录
                info.name = jsonObjectData.value("name").toString();
                info.fulladdress = jsonObjectData.value("ip").toString();
                info.port = jsonObjectData.value("port").toString().toInt();
                info.username = jsonObjectData.value("username").toString();
                info.password = jsonObjectData.value("password").toString();
                info.domain = jsonObjectData.value("domain").toString();

                emit localLogin(info);
                return;
            }

            break;

		case 3:
		{
			QStringList list = jsonObjectData.value("filepath").toString().split("\\");
			info.type = type;
			info.name = list[list.size() - 1];
			info.fulladdress = jsonObjectData.value("ip").toString();
			info.port = jsonObjectData.value("port").toString().toInt();
			info.username = jsonObjectData.value("username").toString();
			info.password = jsonObjectData.value("password").toString();
			info.domain = jsonObjectData.value("domain").toString();
			info.filepath = jsonObjectData.value("filepath").toString();

			emit textReceived(info, msg);
			break;
		}

        // Destkop live
        case 4:
            info.type = type;
            info.desktopLiveId = jsonObjectData.value("bizid").toString();
            info.fulladdress = jsonObjectData.value("ip").toString();
            info.port = jsonObjectData.value("port").toString().toInt();
            info.name = jsonObjectData.value("desktopname").toString();
            info.username = jsonObjectData.value("username").toString();
            info.password = jsonObjectData.value("password").toString();
            info.domain = jsonObjectData.value("domain").toString();
            info.subject = jsonObjectData.value("subject").toString();

            emit textReceived(info, msg);
            break;

        // Receive file from xiaohui
        case 5:
            info.type = type;
            url = jsonObjectData.value("url").toString();
            filename = jsonObjectData.value("filename").toString();

            m_loginDataHandler->downloadFile(info, filename, url);
            break;

        // VNC/SSH desktop
        case 6:
        case 8:
			info.type = type;//rdpDesktopType;
            info.fulladdress = jsonObjectData.value("ip").toString();
            info.port = jsonObjectData.value("port").toString().toInt();
			info.name = jsonObjectData.value("name").toString();
            info.username = jsonObjectData.value("username").toString();
            info.password = jsonObjectData.value("password").toString();
            info.domain = jsonObjectData.value("domain").toString();
            info.msg = msg;

            emit textReceived(info, msg);
            break;
        
        // H5 chat login
        case 7:
#ifdef _DEBUG
            qDebug() << msg;
#endif
			info.type = type;
            token = jsonObjectData.value("token").toString();
            tokenEncode = QString(QUrl::toPercentEncoding(token));
            chatServerApi = jsonObjectData.value("chatserverapi").toString();
            chatServerApiEncode = QString(QUrl::toPercentEncoding(chatServerApi));
            openfireBosh = jsonObjectData.value("openfirebosh").toString();
            openfireBoshEncode = QString(QUrl::toPercentEncoding(openfireBosh));
            chatServerId = jsonObjectData.value("chatserverId").toString();
            chatServerIdEncode = QString(QUrl::toPercentEncoding(chatServerId));
            companyName = jsonObjectData.value("companyname").toString();
            companyNameEncode = QString(QUrl::toPercentEncoding(companyName));
            chatletCache = jsonObjectData.value("openfireChatletCache").toString();
            chatletCacheEncode = QString(QUrl::toPercentEncoding(chatletCache));

            urlHost = m_pdChatHost + "/login/goChatHtml5/chatserverinfo?";
            url = urlHost + "token=" + tokenEncode + "&chatserverapi=" + chatServerApiEncode +
                "&openfirebosh=" + openfireBoshEncode + "&chatserverId=" + chatServerIdEncode +
                "&companyName=" + companyNameEncode + "&openfireChatletCache=" + chatletCacheEncode;

			emit textReceived(info, url);
            break;

        // Space login
        case 14:
            //QString strData = QString(QJsonDocument(jsonObjectData).toJson());
            m_loginDataHandler->getSpaceLoginUrl(QString(QJsonDocument(jsonObjectData).toJson()));

            break;

        // Second scan login
        case 16:
            qDebug() << 16;
            info.type = type;
            info.xiaohui = jsonObjectData.value("xiaohui").toString();
            info.prekey = jsonObjectData.value("prekey").toString();

            emit textReceived(info, msg);

            break;
        }

		//新架构
		emit oneCloudDataReceived(type, jsonObjectData);
	}

    if (!m_isLogin) {
        m_isLogin = true;
        //获取头像信息
        HttpClient(m_chatSvrInfo.chatServerApi + "/business/user/get")
            .addParam("token", m_chatSvrInfo.token)
            .addHeader("Oc_Platform_AppVersion", "3.0.0")
            .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
            .get([=](const QString &response) mutable {
#ifdef _DEBUG
            qDebug() << response;
#endif

            QJsonParseError jsonerror;
            QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

            if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
                QJsonObject jsonObject = document.object();
                int code = document.object().value("code").toInt();
                if (0 == code) {
                    QString url = jsonObject.value("avatar_url").toString();
                    QString name = jsonObject.value("nick_name").toString();
                    QString mobile = jsonObject.value("mobile").toString();
                    emit userInfo(url, name, mobile);
                    /*QJsonArray jsonObjectData = jsonObject.value("data").toArray();
                    for (int i = 0; i < jsonObjectData.count(); i++) {

                    }*/
                }
            }
        });
    }

    emit login();
    return;
}

void WebSocket::onError(QAbstractSocket::SocketError error)
{
	qDebug() << error;
}
void WebSocket::onSslErrors(const QList<QSslError>& errors)
{
	QList<QSslError>::ConstIterator ci;
	for (ci = errors.constBegin(); ci != errors.constEnd(); ++ci) {
		qDebug() << *ci;
	}
}
//断开连接会启动定时器，触发这个槽函数重新连接
void WebSocket::reconnect()
{
	this->abort();
	_uuid = getRandomString(8) + "_" + mRemoteControlData.mac;
	this->open(QUrl(QString("wss://%1/connection/wait?key=%2").arg(website, _uuid)));
}

void WebSocket::updataUrl()
{
	if (qrTimer->isActive()) {
		createUrlFromWebTime();
		//emit showUrl(QString("https://%1/s?T=10&k=%2&t=%3").arg(website, _uuid, QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch())));
	}
	else {
		reconnect();
	}
}

void WebSocket::requestFinished(QNetworkReply *)
{
}

void WebSocket::onLoginComplete()
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
    QJsonObject rootObj;
    rootObj.insert("action", "login_fb");
    rootObj.insert("success", true);
    rootObj.insert("message", "...");
    QJsonObject dataObj;
    dataObj.insert("companyname", mRemoteControlData.companyName);
    dataObj.insert("nearby_enable", mRemoteControlData.nearbyEnable);
    dataObj.insert("punch_need", "false");
    dataObj.insert("mac", mRemoteControlData.mac);
    dataObj.insert("aliases", mRemoteControlData.aliases);
    dataObj.insert("user_name", mRemoteControlData.userName);
    dataObj.insert("host_name", mRemoteControlData.hostName);
    dataObj.insert("longitude", mRemoteControlData.longitude);
    dataObj.insert("latitude", mRemoteControlData.latitude);
    dataObj.insert("xiaohui", mRemoteControlData.xiaohui);

    rootObj.insert("data", dataObj);

    QString feedbackData = QString(QJsonDocument(rootObj).toJson());

    //QString dataEncode = QString("{\"action\":\"login_fb\",\"success\":true,\"message\":\"...\", %1}").arg(feedbackData);

	//qDebug() << QString("https://%1/connection/feedback/send?key=%2&data=%3").arg(website, _uuid, QString("{\"action\":\"login_fb\",\"success\":true,\"message\":\"...\"}").toUtf8().toPercentEncoding());
    if (!mRemoteControlData.longitude.isEmpty()) {
        QString baseUrl = QString("https://%1/connection/feedback/send").arg(website);

        HttpClient(baseUrl)
            .addParam("key", _uuid)
            .addParam("data", feedbackData)
            .post([=](const QString &response) mutable {
            qDebug() << response;
        },
                [=](const QString &response) mutable {
            qDebug() << response;
        }
        );

        //this->sendTextMessage(QString("https://%1/connection/feedback/send?key=%2&data=%3").arg(website, _uuid, feedbackData.toUtf8().toPercentEncoding()));
    }
    else {
        this->sendTextMessage(QString("https://%1/connection/feedback/send?key=%2&data=%3").arg(website, _uuid, QString("{\"action\":\"login_fb\",\"success\":true,\"message\":\"...\"}").toUtf8().toPercentEncoding()));
    }
}

QString WebSocket::getRandomString(int length)
{
	qsrand(QDateTime::currentMSecsSinceEpoch());//为随机值设定一个seed

	const char chrs[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789";
	int chrs_size = sizeof(chrs);

	char* ch = new char[length + 1];
	memset(ch, 0, length + 1);
	int randomx = 0;
	for (int i = 0; i < length; ++i)
	{
		randomx = rand() % (chrs_size - 1);
		ch[i] = chrs[randomx];
	}

	QString ret(ch);
	delete[] ch;
	return ret;
}

void WebSocket::createUrlFromWebTime()
{
	HttpClient("https://"+website + "/business/sys/gettime")
		//.addHeader("Oc_Platform_AppVersion", "3.0.0")
		//.addHeader("Oc_Chatserver_MinVersion", "3.0.0")
		.get([=](const QString &response) mutable {
		qDebug() << response;

		QJsonParseError jsonerror;
		QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

		if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
			QJsonObject jsonObject = document.object();
			int code = document.object().value("code").toInt();
			if (0 == code) {
				QString time = QString::number(jsonObject.value("time").toDouble(),10,0);
				//QString localTime = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());
				qrTimer->stop();
				qrTimer->start(50000);
				emit showUrl(QString("https://%1/s?T=10&k=%2&t=%3").arg(website, _uuid, time));
			}
		}
	});

	/*QNetworkRequest request;
	QNetworkAccessManager* naManager = new QNetworkAccessManager(this);
	QMetaObject::Connection connRet = QObject::connect(naManager, SIGNAL(finished(QNetworkReply*)), this, SLOT(requestFinished(QNetworkReply*)));
	Q_ASSERT(connRet);

	request.setUrl(QUrl("https://www.baidu.com"));
	QNetworkReply* reply = naManager->get(request);*/
}
