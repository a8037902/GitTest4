#include <QJsonobject>
#include <QJsonArray>
#include <QUrl>
#include "smsh5login.h"
#include "httpclient.h"
#include "settings.h"
#include "onecloudmessagebox.h"

const QString SMS_LOGIN_URL = "/business/user/login";
const QString SMS_SEND_URL = "/business/sms/send";

void SmsH5Login::Login(LoginData &loginData,
    std::function<void(QString &url, ChatServerInfo &chatSvrInfo)> successHandler,
    std::function<void()> errorHandler)
{
    ChatServerInfo chatSvrInfo = chatServerInfo();

    if (chatSvrInfo.chatServerApi.isEmpty())
        return;

    QString reqUrl = chatSvrInfo.chatServerApi + SMS_LOGIN_URL;

    QJsonObject rootObj;
    rootObj.insert("mobile", loginData.mobile);
    rootObj.insert("code", loginData.smsCode);
    rootObj.insert("platform_type", loginData.platformType);

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
                OneCloudMessageBox::warning(NULL, QStringLiteral("����"), msg);

                if (errorHandler) {
                    errorHandler();
                }
            }
        }
    }, [=](const QString &resp) {
        OneCloudMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("���ӷ��������� "));
        if (errorHandler) {
            errorHandler();
        }
    });
}

void SmsH5Login::sendSms(QString &mobile,
    std::function<void()> successHandler,
    std::function<void()> errorHandler)

{
    ChatServerInfo chatSvrInfo = chatServerInfo();

    if (chatSvrInfo.chatServerApi.isEmpty()) {
        if (errorHandler) {
            errorHandler();
        }
        return;
    }

    QString reqUrl = chatSvrInfo.chatServerApi + SMS_SEND_URL;

    HttpClient(reqUrl)
        .addParam("mobile", mobile)
        .addHeader("Oc_Platform_AppVersion", "3.0.0")
        .addHeader("Oc_Chatserver_MinVersion", "3.0.0")
        .post([=](const QString &response) mutable {

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();

            int code = document.object().value("code").toInt();
            if (code != 0) {
                QString msg = jsonObject.value("message").toString();
                OneCloudMessageBox::warning(NULL, QStringLiteral("����"), msg);

                if (errorHandler) {
                    errorHandler();
                }
            }
            else {
                if (successHandler) {
                    successHandler();
                }
            }
        }
    }, [=](const QString &resp) {
        OneCloudMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("���ӷ��������� "));
        if (errorHandler) {
            errorHandler();
        }
    });
}
