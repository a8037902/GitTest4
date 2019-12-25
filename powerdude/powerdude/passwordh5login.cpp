#include <QDebug>
#include <QJsonDocument>
#include <QJsonobject>
#include <QJsonArray>
#include <QUrl>
#include "passwordh5login.h"
#include "httpclient.h"
#include "settings.h"
#include "onecloudmessagebox.h"

const QString PASSWROD_LOGIN_URL = QStringLiteral("/business/user/login1");

void PasswordH5Login::Login(LoginData &loginData,
    std::function<void(QString &url, ChatServerInfo &chatSvrInfo)> successHandler,
    std::function<void()> errorHandler)
{
    ChatServerInfo chatSvrInfo = chatServerInfo();

    if (chatSvrInfo.chatServerApi.isEmpty())
        return;

    QString reqUrl = chatSvrInfo.chatServerApi + PASSWROD_LOGIN_URL;

    QJsonObject rootObj;
    //添加键值对，值的类型自动识别，顺序不可自定义
    rootObj.insert("account", loginData.account);
    rootObj.insert("password", loginData.password);
    rootObj.insert("platform_type",loginData.platformType);

    QString strData = QString(QJsonDocument(rootObj).toJson());

    HttpClient(reqUrl)
        .addParam(strData)
        .addHeader("content-type", "application/json")
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();

            int code = document.object().value("code").toInt();
            if (code == 0) {
                QString token = jsonObject.value("token").toString();

                QString chatserverapiEncode = QString(QUrl::toPercentEncoding(chatServerInfo().chatServerApi));
                QString openfireboshEncode = QString(QUrl::toPercentEncoding(chatServerInfo().openfireBosh));
                QString chatserverIdEncode = QString(QUrl::toPercentEncoding(chatServerInfo().chatServerId));
                QString companyNameEncode = QString(QUrl::toPercentEncoding(chatServerInfo().chatServerName));
                QString openfireChatletCacheEncode = QString(QUrl::toPercentEncoding(chatServerInfo().openfireChatletCache));

                SingleTon<Settings>::getInstance().beginGroup("Globa");
                QString chatHost = SingleTon<Settings>::getInstance().value("pdchat_host", "https://pdchat.pispower.com").toString();
                SingleTon<Settings>::getInstance().endGroup();

                QString url = chatHost + LOGIN_FORWARD_URL + "?" +
                    "token=" + token + "&chatserverapi=" + chatserverapiEncode +
                    "&openfirebosh=" + openfireboshEncode + "&chatserverId=" + chatserverIdEncode +
                    "&companyName=" + companyNameEncode + "&openfireChatletCache=" + openfireChatletCacheEncode;

                qDebug() << url;

                ChatServerInfo chatSvrInfo;
                chatSvrInfo.chatServerApi = chatServerInfo().chatServerApi;
                chatSvrInfo.chatServerId = chatServerInfo().chatServerId;
                chatSvrInfo.chatServerName = chatServerInfo().chatServerName;
                chatSvrInfo.openfireBosh = chatServerInfo().openfireBosh;
                chatSvrInfo.openfireChatletCache = chatServerInfo().openfireChatletCache;
                chatSvrInfo.token = token;

                setChatServerInfo(chatSvrInfo);

                if (successHandler) {
                    successHandler(url, chatServerInfo());
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
    }, [=](const QString &resp) {
        OneCloudMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("连接服务器出错！ "));
        if (errorHandler) {
            errorHandler();
        }
    });
}

