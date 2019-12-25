#pragma once
#include <qobject.h>
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include "axwidgetmanage.h"

class VncSshDesktop : public QObject
{
    Q_OBJECT
public:
    explicit VncSshDesktop();
    ~VncSshDesktop() {};

	QString getLoginUrl(QString &key,QString &msg);
    void getDesktopLiveUrl(QString &key, QString &msg);

signals:
	void networkComplete(QString &key, QString &url);
private slots:
    void onNetworkComplete(QNetworkReply *reply);
    

private:
    QNetworkRequest request;
    QString m_loginUrl;

	QMap<QNetworkReply*, QString> keyMap;

    QString m_pdHost;
};