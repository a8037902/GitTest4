#include <QDebug.h>
#include <QJsonDocument.h>
#include <QJsonobject.h>
#include <QJsonArray.h>
#include "settings.h"
#include "h5login.h"
#include "httpclient.h"
#include "onecloudmessagebox.h"

const QString GET_CHATSERVER_INFO_URL = "/hottub/chatserver/getByDomain";
const QString GET_CHATSERVER_INFO_LIST_URL = "/hottub/chatserver/searchByDomain";

H5Login::H5Login(QObject *parent)
    : QObject(parent)
{
    getChatServerInfoList();
}

void H5Login::setChatServerInfo(const ChatServerInfo &chatServerInfo)
{
    mChatServerInfo = chatServerInfo;
}

void H5Login::setChatServerInfoList(const QList<ChatServerInfo> &chatServerInfoList)
{
    mChatServerInfoList = chatServerInfoList;
}

QString H5Login::constructLoginUrl(ChatServerInfo &chatServerInfo, QString &token)
{
    return QString("");
}

void H5Login::getChatServerInfoByDomain(QString &domain,
    std::function<void(const ChatServerInfo &chatSvrInfo)> finishHandler,
    std::function<void()> errorHandler)
{
    SingleTon<Settings>::getInstance().beginGroup("Globa");
    QString appccHost = SingleTon<Settings>::getInstance().value("website", "appcc.cloudak47.com").toString();
    SingleTon<Settings>::getInstance().endGroup();
    QString reqUrl = "https://" +  appccHost + GET_CHATSERVER_INFO_URL;

    // [[3]] POST 请求有参数，有自定义 header
    HttpClient(reqUrl)
        .addParam("domain", domain)
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            int code = jsonObject.value("code").toInt();
            if (0 == code) {
                ChatServerInfo chatSvrInfo;

                chatSvrInfo.chatServerName = jsonObject.value("company_name").toString();
                chatSvrInfo.chatServerApi = jsonObject.value("chatserver_api").toString();
                chatSvrInfo.chatServerId = QString::number(jsonObject.value("chatserver_id").toInt());
                chatSvrInfo.openfireChatletCache = jsonObject.value("openfire_chatlet_cache").toString();
                chatSvrInfo.openfireBosh = jsonObject.value("openfire_bosh").toString();

                setChatServerInfo(chatSvrInfo);

                if (finishHandler) {
                    finishHandler(chatSvrInfo);
                }
            }
            else {
                QString msg = jsonObject.value("message").toString();
                OneCloudMessageBox::warning(NULL, QStringLiteral("警告"), msg);
                if (errorHandler) {
                    errorHandler();
                }
            }
        }
    },
            [=](const QString & resp) {
        OneCloudMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("连接服务器出错！ "));
        if (errorHandler) {
            errorHandler();
        }
    }
    );
}

void H5Login::getChatServerInfoList(
    std::function<void(const QList<ChatServerInfo> &chatSvrInfoList)> finishHandler,
    std::function<void()> errorHandler)
{
    SingleTon<Settings>::getInstance().beginGroup("Globa");
    QString appccHost = SingleTon<Settings>::getInstance().value("website", "appcc.cloudak47.com").toString();
    SingleTon<Settings>::getInstance().endGroup();
    QString reqUrl = "https://" + appccHost + GET_CHATSERVER_INFO_LIST_URL;
   
    HttpClient(reqUrl)
        .get([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            int code = jsonObject.value("code").toInt();
            if (0 == code) {
                QJsonArray jsonArrayData = jsonObject.value("data").toArray();

                QList<ChatServerInfo> infoList;
                for (int i = 0; i < jsonArrayData.size(); i++) {
                    ChatServerInfo chatSvrInfo;

                    chatSvrInfo.chatServerName = jsonArrayData.at(i).toObject().value("company_name").toString();
                    chatSvrInfo.chatServerApi = jsonArrayData.at(i).toObject().value("chatserver_api").toString();
                    chatSvrInfo.chatServerId = QString::number(jsonArrayData.at(i).toObject().value("chatserver_id").toInt());
                    chatSvrInfo.openfireChatletCache = jsonArrayData.at(i).toObject().value("openfire_chatlet_cache").toString();
                    chatSvrInfo.openfireBosh = jsonArrayData.at(i).toObject().value("openfire_bosh").toString();

                    chatSvrInfo.recommendImageUrl = jsonArrayData.at(i).toObject().value("recommend_image").toString();
                    infoList.append(chatSvrInfo);
                }

                if (finishHandler) {
                    setChatServerInfoList(infoList);
                    finishHandler(infoList);
                }
            }
            else {
                QString msg = jsonObject.value("message").toString();
                OneCloudMessageBox::warning(NULL, QStringLiteral("警告"), msg);
                if (errorHandler) {
                    errorHandler();
                }
            }
        }
    },
            [=](const QString & resp) {
        OneCloudMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("连接服务器出错！ "));
        if (errorHandler) {
            errorHandler();
        }
    }
    );
}