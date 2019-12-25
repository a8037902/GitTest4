#pragma once
#include <qobject.h>
#include <QNetworkReply>
#include <QNetworkAccessManager>

class Update :public QObject
{
	Q_OBJECT
public:
	explicit Update();
	~Update() {};

	bool check_updatefile(const QString& url);
	QString query_version(const QString& url);
	bool compare(const QString& x, const QString& y);

	QString file_path;

signals:
	void getVersion(QString &version);

private slots:
	//axwidget event
	void onNetworkComplete(QNetworkReply *reply);
private:
	QString m_latestVersion;
	QNetworkRequest request;
	
};