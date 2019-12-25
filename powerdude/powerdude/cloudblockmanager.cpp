#include <QDebug.h>
#include <QJsonDocument.h>
#include <QJsonobject.h>
#include <QJsonArray.h>
#include <QUrl.h>
#include <QNetworkRequest.h>
#include <QNetworkAccessManager.h>
#include <QNetworkReply.h>
#include <QMessageBox>
#include "cloudblockmanager.h"
#include "httpclient.h"
#include "settings.h"
#include "chatsvrinfo.h"

CloudBlockManager::CloudBlockManager()    
{
    m_vncSshDesktop = new VncSshDesktop();
}

QList<MstscRdpInfo> CloudBlockManager::getDesktopList(ChatServerInfo &info, QString url)
{
    QList<MstscRdpInfo> list;
    QString reqUrl = info.chatServerApi + url;

    // [[3]] POST 请求有参数，有自定义 header
    HttpClient(reqUrl)
        .addParam("token", info.token)
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            int code = document.object().value("code").toInt();
            if (0 == code) {
                QJsonArray jsonObjectData = jsonObject.value("data").toArray();
                for (int i = 0; i < jsonObjectData.count(); i++) {
                    MstscRdpInfo info;

                    info.type = 2;   // 桌面
                    info.desktopType = RDPDestkopType(jsonObjectData.at(i).toObject().value("type").toInt());

                    // Ignore face auth desktop
                    if (info.desktopType == RDPDestkopType::FaceAuthDesktop) {
                        continue;
                    }

                    info.name = jsonObjectData.at(i).toObject().value("name").toString();
                    info.fulladdress = jsonObjectData.at(i).toObject().value("ip").toString();
                    info.port = jsonObjectData.at(i).toObject().value("port").toInt();
                    info.username = jsonObjectData.at(i).toObject().value("username").toString();
                    info.password = jsonObjectData.at(i).toObject().value("password").toString();
                    info.domain = jsonObjectData.at(i).toObject().value("domain").toString();
                    info.p2pId = jsonObjectData.at(i).toObject().value("p2p_id").toString();

                    list.append(info);
                }

                emit textReceived(list);
            }
        }
    });

    return list;
}

void CloudBlockManager::getDesktopFileList(ChatServerInfo &info, QList<MstscRdpInfo>& list, QList<int> listDesktopId, int index)
{
	HttpClient(info.chatServerApi + "/business/user/desktop/get")
		.addParam("token", info.token)
		.addParam("desktop_id", QString::number(listDesktopId[index]))
		.addHeader("Oc_Platform_AppVersion", "3.0.0")
		.addHeader("Oc_Chatserver_MinVersion", "3.0.0")
		.post([=](const QString &response) mutable {
		qDebug() << response;

		QJsonParseError jsonerror;
		QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

		if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
			QJsonObject jsonObject = document.object();
			int code = document.object().value("code").toInt();
			if (0 == code) {
				QJsonObject jsonObjectData = jsonObject.value("data").toObject();

				//list[index].name = jsonObjectData.value("file_name").toString();
				list[index].fulladdress = jsonObjectData.value("ip").toString();
				list[index].port = jsonObjectData.value("port").toInt();
				list[index].username = jsonObjectData.value("username").toString();
				list[index].password = jsonObjectData.value("password").toString();
				list[index].domain = jsonObjectData.value("domain").toString();
				index++;

				if (index >= list.size()) {
					emit textReceived(list);
				}
				else {
					getDesktopFileList(info, list, listDesktopId, index);
				}
			}
		}
	});
}

QList<MstscRdpInfo> CloudBlockManager::getDesktopFileList(ChatServerInfo &info, QString url)
{
	QList<MstscRdpInfo> list;
	QList<int> listDesktopId;
	int index = 0;
	QString reqUrl = info.chatServerApi + url;

	// [[3]] POST 请求有参数，有自定义 header
	HttpClient(reqUrl)
		.addParam("token", info.token)
		.addHeader("Oc_Platform_AppVersion", "3.0.0")
		.addHeader("Oc_Chatserver_MinVersion", "3.0.0")
		.post([=](const QString &response) mutable {
		qDebug() << response;

		QJsonParseError jsonerror;
		QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

		if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
			QJsonObject jsonObject = document.object();
			int code = document.object().value("code").toInt();
			if (0 == code) {
				QJsonArray jsonObjectData = jsonObject.value("data").toArray();
				for (int i = 0; i < jsonObjectData.count(); i++) {
					MstscRdpInfo info;

					info.type = 3;   // 桌面文件

					info.name = jsonObjectData.at(i).toObject().value("file_name").toString();
					info.fulladdress = jsonObjectData.at(i).toObject().value("ip").toString();
					info.port = jsonObjectData.at(i).toObject().value("port").toInt();
					info.username = jsonObjectData.at(i).toObject().value("username").toString();
					info.password = jsonObjectData.at(i).toObject().value("password").toString();
					info.domain = jsonObjectData.at(i).toObject().value("domain").toString();
					info.filepath = jsonObjectData.at(i).toObject().value("file_path").toString();

					listDesktopId.append(jsonObjectData.at(i).toObject().value("desktop_id").toInt());
					list.append(info);
				}

				if(jsonObjectData.count()>0)getDesktopFileList(info, list, listDesktopId,0);

				//emit textReceived(list);
			}
		}
	});

	return list;
}


QList<MstscRdpInfo> CloudBlockManager::getVncDesktopList(ChatServerInfo &info, QString url)
{
    QList<MstscRdpInfo> list;
    QString reqUrl = info.chatServerApi + url;

    HttpClient(reqUrl)
        .addParam("token", info.token)
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            int code = document.object().value("code").toInt();
            if (0 == code) {
                QJsonArray jsonObjectData = jsonObject.value("data").toArray();
                for (int i = 0; i < jsonObjectData.count(); i++) {
                    MstscRdpInfo rdpInfo;

                    rdpInfo.type = 8; //vnc type
                    rdpInfo.name = jsonObjectData.at(i).toObject().value("name").toString();
                    rdpInfo.fulladdress = jsonObjectData.at(i).toObject().value("ip").toString();
                    rdpInfo.port = jsonObjectData.at(i).toObject().value("port").toInt();
                    rdpInfo.username = jsonObjectData.at(i).toObject().value("username").toString();
                    rdpInfo.password = jsonObjectData.at(i).toObject().value("password").toString();
                    rdpInfo.domain = jsonObjectData.at(i).toObject().value("domain").toString();

                    int id = jsonObjectData.at(i).toObject().value("id").toInt();
                    QString share = jsonObjectData.at(i).toObject().value("share").toString();

                    // 创建JSON对象
                    QJsonObject rootObj;
                    //添加键值对，值的类型自动识别，顺序不可自定义
                    rootObj.insert("action", "login");
                    rootObj.insert("type", 8);
                    QJsonObject dataObj;
                    dataObj.insert("bizid", QString::number(id));
                    dataObj.insert("ip", rdpInfo.fulladdress);
                    dataObj.insert("port", QString::number(rdpInfo.port));
                    dataObj.insert("companyname", info.chatServerName);
                    dataObj.insert("password", rdpInfo.password);
                    dataObj.insert("xiaohui", info.xiaohui);
                    dataObj.insert("share", share);
                    rootObj.insert("data", dataObj);

                    QString strData = QString(QJsonDocument(rootObj).toJson());
                    rdpInfo.msg = strData;
                    list.append(rdpInfo);
                }
                emit textReceived(list);
            }
        }
    });

    return list;
}

QList<MstscRdpInfo> CloudBlockManager::getSSHDesktopList(ChatServerInfo &info, QString url)
{
    QList<MstscRdpInfo> list;
    QString reqUrl = info.chatServerApi + url;

    HttpClient(reqUrl)
        .addParam("token", info.token)
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            int code = document.object().value("code").toInt();
            if (0 == code) {
                QJsonArray jsonObjectData = jsonObject.value("data").toArray();
                for (int i = 0; i < jsonObjectData.count(); i++) {
                    MstscRdpInfo rdpInfo;

                    rdpInfo.type = 6; //vnc type
                    rdpInfo.name = jsonObjectData.at(i).toObject().value("name").toString();
                    rdpInfo.fulladdress = jsonObjectData.at(i).toObject().value("ip").toString();
                    rdpInfo.port = jsonObjectData.at(i).toObject().value("port").toInt();
                    rdpInfo.username = jsonObjectData.at(i).toObject().value("username").toString();
                    rdpInfo.password = jsonObjectData.at(i).toObject().value("password").toString();
                    rdpInfo.domain = jsonObjectData.at(i).toObject().value("domain").toString();

                    int id = jsonObjectData.at(i).toObject().value("id").toInt();
                    QString share = jsonObjectData.at(i).toObject().value("share").toString();

                    // 创建JSON对象
                    QJsonObject rootObj;
                    //添加键值对，值的类型自动识别，顺序不可自定义
                    rootObj.insert("action", "login");
                    rootObj.insert("type", 6);
                    QJsonObject dataObj;
                    dataObj.insert("bizid", QString::number(id));
                    dataObj.insert("ip", rdpInfo.fulladdress);
                    dataObj.insert("port", QString::number(rdpInfo.port));
                    dataObj.insert("companyname", info.chatServerName);
                    dataObj.insert("password", rdpInfo.password);
                    dataObj.insert("xiaohui", info.xiaohui);
                    dataObj.insert("share", share);
                    rootObj.insert("data", dataObj);

                    QString strData = QString(QJsonDocument(rootObj).toJson());
                    rdpInfo.msg = strData;
                    list.append(rdpInfo);
                    
                }
                emit textReceived(list);
            }
        }
    });

    return list;
}

void CloudBlockManager::getWebsiteList(ChatServerInfo &info, QString url)
{
    QString reqUrl = info.chatServerApi + url;

    HttpClient(reqUrl)
        .addParam("token", info.token)
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {
        qDebug() << response;
        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            int code = document.object().value("code").toInt();

            if (0 == code) {
                QList<MstscRdpInfo> list;
                QJsonArray jsonObjectData = jsonObject.value("data").toArray();

                for (int i = 0; i < jsonObjectData.count(); i++) {
                    MstscRdpInfo info;
                    info.type = 11;      //website type
                    info.websitedata.thirdpartyName = jsonObjectData.at(i).toObject().value("thirdparty_name").toString();
                    info.websitedata.accessUrl = jsonObjectData.at(i).toObject().value("access_url").toString();
                    info.websitedata.bizId = jsonObjectData.at(i).toObject().value("id").toInt();
                    list.append(info);
                }

                emit textReceived(list);
            }
        }
    });

    return;
}

void CloudBlockManager::getH5ChatLoginUrl(ChatServerInfo &info, QString url)
{
    QString reqUrl = info.chatServerApi + url;
    MstscRdpInfo rdpInfo;

    HttpClient(reqUrl)
        .addParam("token", info.token)
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            int code = document.object().value("code").toInt();
            if (0 == code) {
                QString H5token = jsonObject.value("token").toString();
                QString H5tokenEncode = QString(QUrl::toPercentEncoding(H5token));

                QString openfireBosh = jsonObject.value("openfire_bosh").toString();
                QString openfireBoshEncode = QString(QUrl::toPercentEncoding(openfireBosh));

                QString chatletCache = jsonObject.value("openfire_chatlet_cache").toString();
                QString chatletCacheEncode = QString(QUrl::toPercentEncoding(chatletCache));

                QString chatServerApiEncode = QString(QUrl::toPercentEncoding(info.chatServerApi));
                QString chatServerIdEncode = QString(QUrl::toPercentEncoding(info.chatServerId));
                QString companyNameEncode = QString(QUrl::toPercentEncoding(info.chatServerName));

                
                SingleTon<Settings>::getInstance().beginGroup("Globa");
                QString chatHost = SingleTon<Settings>::getInstance().value("pdchat_host", "https://pdchat.pispower.com").toString();
                SingleTon<Settings>::getInstance().endGroup();
                QString urlHost = chatHost + "/login/goChatHtml5/chatserverinfo?";
				QString url = urlHost + "token=" + H5tokenEncode + "&chatserverapi=" + chatServerApiEncode +
					"&openfirebosh=" + openfireBoshEncode + "&chatserverId=" + chatServerIdEncode +
					"&companyName=" + companyNameEncode + "&openfireChatletCache=" + chatletCacheEncode;

                rdpInfo.type = 7;
                emit textReceived(rdpInfo, url);
            }
        }
    });

    return;
}

void CloudBlockManager::getSpaceLoginUrl(ChatServerInfo &info, QString url)
{
    QString reqUrl = info.chatServerApi + url;
    MstscRdpInfo rdpInfo;

    HttpClient(reqUrl)
        .addParam("token", info.token)
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([this, info](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            int code = document.object().value("code").toInt();
            if (0 == code) {
                QString spaceToken = jsonObject.value("token").toString();
                QString nickname = jsonObject.value("nickname").toString();
                QString avatar = jsonObject.value("avatar").toString();
                QString imUserName = jsonObject.value("im_user_name").toString();

                //创建JSON对象
                QJsonObject root_Obj;
                //添加键值对，值的类型自动识别，顺序不可自定义
                root_Obj.insert("token", spaceToken);
                root_Obj.insert("chatserverapi", info.chatServerApi);
                root_Obj.insert("companyname", info.chatServerName);
                root_Obj.insert("chatserverId", info.chatServerId);
                root_Obj.insert("xiaohui", imUserName);
                root_Obj.insert("nickname", nickname);
                root_Obj.insert("avatar", avatar);

                QString strData = QString(QJsonDocument(root_Obj).toJson());
                QNetworkRequest request;

                SingleTon<Settings>::getInstance().beginGroup("Globa");
                QString m_spaceHost = SingleTon<Settings>::getInstance().value("space_host", "https://space.pispower.com").toString();
                SingleTon<Settings>::getInstance().endGroup();
                QString baseUrl = m_spaceHost + "/space";

                request.setUrl(QUrl(baseUrl));
                request.setHeader(QNetworkRequest::ContentTypeHeader, QVariant("application/json"));
                QByteArray params = strData.toUtf8();

                QNetworkAccessManager *manager = new QNetworkAccessManager(this);
                //QEventLoop eventLoop;
                QObject::connect(manager, &QNetworkAccessManager::finished, this, &CloudBlockManager::onNetworkComplete);
                QNetworkReply *reply = manager->post(request, params);
            }
        }

    });
}

void CloudBlockManager::onNetworkComplete(QNetworkReply *reply)
{
    QObject::disconnect(qobject_cast<QNetworkAccessManager *>(sender()), &QNetworkAccessManager::finished, this, &CloudBlockManager::onNetworkComplete);
    QJsonParseError jsonerror;
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &jsonerror);
    QByteArray bytes = reply->readAll();
    QString str = QString::fromUtf8(bytes.data(), bytes.size());
    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    MstscRdpInfo info;
    qDebug() << str;

    {
        if (!document.isNull() && !jsonerror.error == QJsonParseError::NoError) {
            return;
        }

        QJsonObject jsonObject = document.object();
        int code = document.object().value("code").toInt();
        QString key = document.object().value("key").toString();

        SingleTon<Settings>::getInstance().beginGroup("Globa");
        QString m_spaceHost = SingleTon<Settings>::getInstance().value("space_host", "https://space.pispower.com").toString();
        SingleTon<Settings>::getInstance().endGroup();
        QString baseUrl = m_spaceHost + "/space" + "?key=" + key;
        info.type = 14;

        emit textReceived(info, baseUrl);
        return;
    }
}

QList<MstscRdpInfo> CloudBlockManager::getDesktopLiveList(ChatServerInfo &info, QString url)
{
    QList<MstscRdpInfo> list;
    QString reqUrl = info.chatServerApi + url;

    HttpClient(reqUrl)
        .addParam("token", info.token)
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            QString strData = QString(QJsonDocument(jsonObject).toJson());
            qDebug() << strData;

            int code = document.object().value("code").toInt();
            if (0 == code) {
                QJsonArray jsonObjectData = jsonObject.value("data").toArray();
                for (int i = 0; i < jsonObjectData.count(); i++) {
                    QString strDebug = QString(QJsonDocument(jsonObjectData).toJson());
                    qDebug() << strDebug;

                    MstscRdpInfo rdpInfo;
                    rdpInfo.type = 4;  // destkop live type
                    rdpInfo.desktopLiveId = QString::number(jsonObjectData.at(i).toObject().value("id").toInt());
                    rdpInfo.subject = jsonObjectData.at(i).toObject().value("subject").toString();
                    rdpInfo.host = jsonObjectData.at(i).toObject().value("host").toBool();
                    rdpInfo.hostStatus = jsonObjectData.at(i).toObject().value("host_status").toInt();
                    rdpInfo.name = jsonObjectData.at(i).toObject().value("desktop_name").toString();

                    list.append(rdpInfo);
                }
                emit textReceived(list);
            }
        }
    });

    return list;
}


void CloudBlockManager::constructSingleDesktopLiveData(ChatServerInfo &chatSvrInfo, QString &desktopLiveId, QString &key)
{
    QString url = chatSvrInfo.chatServerApi + "/business/user/desktop/play/login";

    HttpClient(url)
        .addFormHeader()
        .addParam("desktop_play_id", desktopLiveId)
        .addParam("token", chatSvrInfo.token)
		.addHeader("Oc_Platform_AppVersion", "3.0.0")
		.addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
			qDebug()<< QString(QJsonDocument(jsonObject).toJson());
            int code = document.object().value("code").toInt();
            if (0 == code) {
                QString bizId = QString::number(jsonObject.value("desktop_play_id").toInt());
                QString avatar = jsonObject.value("avatar").toString();
                QString hostId = QString::number(jsonObject.value("host_desktop_play_id").toInt());
                QString xiaohui = jsonObject.value("xiaohui").toString();
                QString nickname = jsonObject.value("nick_name").toString();
                QString hostnickname = jsonObject.value("host_nick_name").toString();
                QString prestartat = QString::number(jsonObject.value("pre_start_at").toDouble(), 10, 0); 
                QString preendat = QString::number(jsonObject.value("pre_end_at").toDouble(), 10, 0); 
                QString host = jsonObject.value("host").toBool()?"true":"false";
                QString subject = jsonObject.value("subject").toString();
                QString desktopname = jsonObject.value("desktop_name").toString();
                QString catalog = "RDP";
                QString type = QString::number(jsonObject.value("type").toInt());
                QString ip = jsonObject.value("ip").toString();
                QString port = QString::number(jsonObject.value("port").toInt());
                QString domain = jsonObject.value("domain").toString();
                QString username = jsonObject.value("username").toString();
                QString password = jsonObject.value("password").toString();

                QString playtoken;
                if (!host.compare(QString::fromLocal8Bit("false"))) {
                    playtoken = jsonObject.value("play_token").toString();
                }

                QJsonObject rootObj;
                rootObj.insert("action", "login");
                rootObj.insert("type", 4);
                rootObj.insert("chatserverapi", chatSvrInfo.chatServerApi);
                rootObj.insert("chatserverid", chatSvrInfo.chatServerId);
                rootObj.insert("chatservername", chatSvrInfo.chatServerName);
                rootObj.insert("token7", chatSvrInfo.token);

                QJsonObject dataObj;
                dataObj.insert("bizid", bizId);
                dataObj.insert("avatar", avatar);
                dataObj.insert("port", port);
                dataObj.insert("hostbizid", hostId);
                dataObj.insert("xiaohui", xiaohui);
                dataObj.insert("nickname", nickname);
                dataObj.insert("hostnickname", hostnickname);
                dataObj.insert("prestartat", prestartat);
                dataObj.insert("preendat", preendat);
                dataObj.insert("host", host);
                dataObj.insert("subject", subject);
                dataObj.insert("desktopname", desktopname);
                dataObj.insert("catalog", catalog);
                dataObj.insert("type", type);
                dataObj.insert("ip", ip);
                dataObj.insert("port", port);
                dataObj.insert("domain", domain);
                dataObj.insert("username", username);
                dataObj.insert("password", password);

                if (!host.compare(QString::fromLocal8Bit("false"))) {
                    dataObj.insert("playtoken", playtoken);
                }

                rootObj.insert("data", dataObj);

                QString strData = QString(QJsonDocument(rootObj).toJson());

                qDebug() << strData;

                MstscRdpInfo info;
                info.type = 4;  // destkop live type
                info.fulladdress = ip;
                info.port = port.toInt();
                info.username = username;
                info.password = password;
                info.msg = strData;
                info.subject = subject;
                info.domain = domain;

                m_vncSshDesktop->getDesktopLiveUrl(key, strData);
            }
        }
       
    });
}

void CloudBlockManager::getDesktopGroupList(ChatServerInfo &info)
{
    QString url = info.chatServerApi + "/business/user/desktop_group/list";
    QList<MstscRdpInfo> mstscRdpInfoList;
    QList<int> desktopGroupIdList;

    HttpClient(url)
        .addFormHeader()
        .addParam("token", info.token)
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            qDebug() << QString(QJsonDocument(jsonObject).toJson());
            int code = document.object().value("code").toInt();
            if (code != 0) {
                QString msg = document.object().value("msg").toString();
                if (!msg.isEmpty()) {
                    QMessageBox::warning(NULL, QStringLiteral("警告"), msg);
                }
            }
            else {
                QJsonArray jsonArrayData = jsonObject.value("data").toArray();
                for (int i = 0; i < jsonArrayData.count(); i++) {
                    MstscRdpInfo desktopGroupInfo;
                    desktopGroupInfo.desktopGroupData.bizId = QString::number(jsonArrayData.at(i).toObject().value("id").toInt());
                    desktopGroupInfo.desktopGroupData.name = jsonArrayData.at(i).toObject().value("name").toString();
                    desktopGroupInfo.desktopGroupData.count = jsonArrayData.at(i).toObject().value("total_desktop").toInt();
                    desktopGroupInfo.name = jsonArrayData.at(i).toObject().value("name").toString();
                    desktopGroupInfo.type = DESKTOP_GROUP_TYPE;

                    desktopGroupIdList.append(jsonArrayData.at(i).toObject().value("id").toInt());
                    mstscRdpInfoList.append(desktopGroupInfo);
                }
                if (jsonArrayData.count() > 0) {
                    QMap<int, QList<int>> desktopGroupMap;
                    getDesktopGroupInfo(info, mstscRdpInfoList, desktopGroupIdList, desktopGroupMap, 0);
                }

            }
        }
    }, [] (const QString &response) {
        QMessageBox::warning(NULL, QStringLiteral("警告"), response);
    }
    );
}


void CloudBlockManager::getDesktopGroupInfo(ChatServerInfo &info, QList<MstscRdpInfo> &list, QList<int> &desktopGroupIdList,
    QMap<int, QList<int>> &desktopGroupMap, int index)
{
    QString url = info.chatServerApi + "/business/user/desktop_group/desktop/list";

    HttpClient(url)
        .addFormHeader()
        .addParam("token", info.token)
        .addParam("desktop_group_id", QString::number(desktopGroupIdList[index]))
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            qDebug() << QString(QJsonDocument(jsonObject).toJson());
            int code = document.object().value("code").toInt();
            if (code != 0) {
                QString msg = document.object().value("msg").toString();
                if (!msg.isEmpty()) {
                    QMessageBox::warning(NULL, QStringLiteral("警告"), msg);
                }
            }
            else {
                QJsonArray jsonArrayData = jsonObject.value("data").toArray();
                QList<int> desktopIdList;
                for (int i = 0; i < jsonArrayData.count(); i++) {
                    int desktopId = jsonArrayData.at(i).toObject().value("id").toInt();
                    desktopIdList.append(desktopId);
                }
                desktopGroupMap.insert(desktopGroupIdList[index], desktopIdList);

                index++;
                if (index >= desktopGroupIdList.size()) {
                    QMap<int, QList<int>>::const_iterator i = desktopGroupMap.constBegin();
                    getDesktopInfo(info, list, desktopGroupMap, i, 0, 0);
                    qDebug() << "xxx";
                } else {
                    getDesktopGroupInfo(info, list, desktopGroupIdList, desktopGroupMap, index);
                }
            }

        }
    }, [](const QString &response) {
        QMessageBox::warning(NULL, QStringLiteral("警告"), response);
    }
    );
}

void CloudBlockManager::getDesktopInfo(ChatServerInfo &chatSvrinfo, QList<MstscRdpInfo> &list,
    QMap<int,QList<int>> &desktopGroupMap, QMap<int, QList<int>>::const_iterator &i, int index, int desktopIndex)
{
    QString url = chatSvrinfo.chatServerApi + "/business/user/desktop/get";

    HttpClient(url)
        .addFormHeader()
        .addParam("token", chatSvrinfo.token)
        .addParam("desktop_id", QString::number((i.value())[desktopIndex]))
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            qDebug() << QString(QJsonDocument(jsonObject).toJson());
            int code = document.object().value("code").toInt();
            if (code != 0) {
                QString msg = document.object().value("msg").toString();
                if (!msg.isEmpty()) {
                    QMessageBox::warning(NULL, QStringLiteral("警告"), msg);
                }
            }
            else {
                QJsonObject jsonObjectData = jsonObject.value("data").toObject();
                MstscRdpInfo info;
                info.name = jsonObjectData.value("name").toString();
                info.fulladdress = jsonObjectData.value("ip").toString();
                info.port = jsonObjectData.value("port").toInt();
                info.username = jsonObjectData.value("username").toString();
                info.password = jsonObjectData.value("password").toString();
                info.domain = jsonObjectData.value("domain").toString();
                info.p2pId = jsonObjectData.value("p2p_id").toString();
                QString desktopId = QString("%1@%2:%3").arg(info.username).arg(info.fulladdress).arg(info.port);
                list[index].desktopKeyList.append(desktopId);

                if (index == (list.size() - 1)  && desktopIndex == (i.value().size() - 1)) {
                    emit textReceived(list);
                }
                
                else if (desktopIndex < i.value().size()) {   //桌面组index小于桌面组中的桌面size
                    desktopIndex++;
                    if (desktopIndex == i.value().size()) {
                        desktopIndex = 0;
                        index++;
                        i++;
                        getDesktopInfo(chatSvrinfo, list, desktopGroupMap, i, index, desktopIndex);
                    }
                    getDesktopInfo(chatSvrinfo, list, desktopGroupMap, i, index, desktopIndex);
                }
            }

        }
    }, [](const QString &response) {
        QMessageBox::warning(NULL, QStringLiteral("警告"), response);
    }
    );
}
