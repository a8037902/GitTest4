
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QEventLoop>
#include "Update.h"

Update::Update(){
	file_path = "";
}

QString Update::query_version(const QString& url) {

	QString baseUrl = url;

	//QNetworkRequest request;
	request.setUrl(QUrl(baseUrl));

	QNetworkAccessManager *manager = new QNetworkAccessManager(this);

	//QEventLoop eventLoop;
	QObject::connect(manager, &QNetworkAccessManager::finished, this, &Update::onNetworkComplete);
	QNetworkReply *reply = manager->get(request);
	//eventLoop.exec();

	return m_latestVersion;
}

bool Update::check_updatefile(const QString& url) {
	QTextStream out(stdout);
	QTcpSocket socket;
	QByteArray buffer;

	QUrl baseUrl = QUrl(url);
	quint16 port = baseUrl.port()== -1 ? 80: baseUrl.port();

	socket.connectToHost(baseUrl.host(), port);
	if (socket.waitForConnected()) {
		//Standard http request
		socket.write("GET / HTTP/1.1\r\n"
			"host: " + baseUrl.host().toUtf8() + "\r\n\r\n");
		if (socket.waitForReadyRead()) {
			while (socket.bytesAvailable()) {
				buffer.append(socket.readAll());
				int packetSize = buffer.size();
				while (packetSize > 0)
				{
					//Output server response for debugging
					out << "[" << buffer.data() << "]" << endl;

					//set Url if 200, 301, or 302 response given assuming that server will redirect
					if (buffer.contains("200 OK") ||
						buffer.contains("302 Found") ||
						buffer.contains("301 Moved")) {
						return true;
					}
					buffer.remove(0, packetSize);
					//packetSize=getPacketSize(buffer);
					packetSize = buffer.size();

				} //while packet size >0
			} //while socket.bytesavail

		} //socket wait for ready read
	}//socket write   
	return false;
}

bool Update::compare(const QString& x, const QString& y)
{
	QStringList versionsX = x.split(".");
	QStringList versionsY = y.split(".");

	int count = qMin(versionsX.count(), versionsY.count());

	for (int i = 0; i < count; ++i) {
		int a = QString(versionsX.at(i)).toInt();
		int b = QString(versionsY.at(i)).toInt();

		if (a > b)
			return true;

		else if (b > a)
			return false;
	}

	return versionsY.count() < versionsX.count();
}

void Update::onNetworkComplete(QNetworkReply *reply)
{
	QObject::disconnect(qobject_cast<QNetworkAccessManager *>(sender()), &QNetworkAccessManager::finished, this, &Update::onNetworkComplete);

	/*QByteArray bytes = pReplay->readAll();
	qDebug() << bytes;*/
	{
		/* There was a network error */
		if (reply->error() != QNetworkReply::NoError) {
			m_latestVersion = "";
			return;
			//return "";
		}

		/* Try to create a JSON document from downloaded data */
		QJsonDocument document = QJsonDocument::fromJson(reply->readAll());

		/* JSON is invalid */
		if (document.isNull()) {
			m_latestVersion = "";
			return;
			//return "";
		}

		/* Get the platform information */
		QJsonObject updates = document.object().value("updates").toObject();
		QJsonObject platform = updates.value("windows").toObject();

		/* Get update information */

		m_latestVersion = platform.value("latest-version").toString();
		file_path = platform.value("download-url").toString();

		//return m_latestVersion;
		emit getVersion(m_latestVersion);
	}
}