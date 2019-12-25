#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <qjsonarray.h>
#include <QEventLoop>
#include "vncsshdesktop.h"
#include "settings.h"
#include "httpclient.h"

VncSshDesktop::VncSshDesktop() 
{   
    m_loginUrl = "";
}

QString VncSshDesktop::getLoginUrl(QString &key, QString &msg) 
{
    SingleTon<Settings>::getInstance().beginGroup("Globa");
    m_pdHost = SingleTon<Settings>::getInstance().value("pd_host", "https://pd.pispower.com").toString();
    SingleTon<Settings>::getInstance().endGroup();

    QString baseUrl = m_pdHost + "/api/windows_client/getForwardData";

    request.setUrl(QUrl(baseUrl));

    QByteArray params = "appccResult=" + msg.toUtf8();

    QNetworkAccessManager *manager = new QNetworkAccessManager(this);
    //QEventLoop eventLoop;
    QObject::connect(manager, &QNetworkAccessManager::finished, this, &VncSshDesktop::onNetworkComplete);
    QNetworkReply *reply = manager->post(request, params);
	keyMap.insert(reply, key);
    return m_loginUrl;
}

void VncSshDesktop::onNetworkComplete(QNetworkReply *reply) 
{
    QObject::disconnect(qobject_cast<QNetworkAccessManager *>(sender()), &QNetworkAccessManager::finished, this, &VncSshDesktop::onNetworkComplete);
    QJsonParseError jsonerror;
    QJsonDocument document = QJsonDocument::fromJson(reply->readAll(), &jsonerror);

    {
        if (!document.isNull() && !jsonerror.error == QJsonParseError::NoError) {
            m_loginUrl = "";
            return;
        }

        /* Get the url information */
        QJsonArray desktops = document.object().value("desktops").toArray();
        QString url = desktops.at(0).toObject().value("url").toString();

        QString url2 = m_pdHost + url;

		QString key = keyMap[reply];
		keyMap.remove(reply);

		emit networkComplete(key, url2);
        return ;
    }
}

void VncSshDesktop::getDesktopLiveUrl(QString &key, QString &msg)
{
    SingleTon<Settings>::getInstance().beginGroup("Globa");
    QString pdHost = SingleTon<Settings>::getInstance().value("pd_host", "https://pd.pispower.com").toString();
    SingleTon<Settings>::getInstance().endGroup();

    QString url = pdHost + "/api/windows_client/auth_screen_desktop_windows";

    HttpClient(url)
        .addFormHeader()
        .addParam("appccResult", msg)
        .post([=](const QString &response) mutable {
        qDebug() << response;

        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();

            int code = jsonObject.value("code").toInt();
            if (0 == code) {
                QJsonArray jsonObjectData = jsonObject.value("desktops").toArray();
                QString loginUrl = jsonObjectData.at(0).toObject().value("url").toString();
                if (!loginUrl.isEmpty() && !loginUrl.isNull()) {
                    loginUrl = pdHost + loginUrl;
                    emit networkComplete(key, loginUrl);
                }
            }
        }
    });


    return;
}
