#include "HttpClient.h"

#include <QDebug>
#include <QFile>
#include <QHash>
#include <QUrlQuery>
#include <QNetworkReply>
#include <QNetworkRequest>
#include <QNetworkAccessManager>
#include <QHttpPart>
#include <QHttpMultiPart>

struct HttpClientPrivate {
    HttpClientPrivate(const QString &url) : url(url), networkAccessManager(NULL), useInternalNetworkAccessManager(true), debug(false) {}

    QString url; // ����� URL
    QUrlQuery params; // ����Ĳ���
    QString jsonString; // Json��ʽ�������
    QHash<QString, QString> headers; // �����ͷ
    QNetworkAccessManager *networkAccessManager;
    bool useInternalNetworkAccessManager; // �Ƿ�ʹ���ڲ��� QNetworkAccessManager
    bool debug;
};

HttpClient::HttpClient(const QString &url) : d(new HttpClientPrivate(url)) {
    // qDebug() << "HttpClient";
}

HttpClient::~HttpClient() {
    // qDebug() << "~HttpClient";
    delete d;
}

HttpClient &HttpClient::useManager(QNetworkAccessManager *manager) {
    d->networkAccessManager = manager;
    d->useInternalNetworkAccessManager = false;
    return *this;
}

// ���� debug Ϊ true ��ʹ�� debug ģʽ������ִ��ʱ�������� URL �Ͳ�����
HttpClient &HttpClient::debug(bool debug) {
    d->debug = debug;
    return *this;
}

// ���Ӳ���
HttpClient &HttpClient::addParam(const QString &name, const QString &value) {
    d->params.addQueryItem(name, value);
    return *this;
}

// ����Json�������
HttpClient &HttpClient::addParam(const QString &jsonString) {
    d->jsonString = jsonString;
    return *this;
}

// ���ӷ���ͷ
HttpClient &HttpClient::addHeader(const QString &header, const QString &value) {
    d->headers[header] = value;
    return *this;
}

HttpClient &HttpClient::addFormHeader() {
    return addHeader("content-type", "application/x-www-form-urlencoded");
}

// ִ�� GET ����
void HttpClient::get(std::function<void(const QString &)> successHandler,
    std::function<void(const QString &)> errorHandler,
    const char *encoding) {
    execute(false, successHandler, errorHandler, encoding);
}

// ִ�� POST ����
void HttpClient::post(std::function<void(const QString &)> successHandler,
    std::function<void(const QString &)> errorHandler,
    const char *encoding) {
    execute(true, successHandler, errorHandler, encoding);
}

// ʹ�� GET �������أ��������ݿɶ�ȡʱ�ص� readyRead(), ����������Ӧ���� readyRead() ������ݱ��浽�ļ�
void HttpClient::download(std::function<void(const QByteArray &)> readyRead,
    std::function<void(const QByteArray &)> finishHandler,
    std::function<void(const QString &)> errorHandler,
    std::function<void(__int64 & bytesReceived, __int64 & bytesTotal)> downloadProgressHandler) {
    // ����� GET ���󣬲��Ҳ�����Ϊ�գ����������Ĳ������ŵ� URL ����
    if (!d->params.isEmpty()) {
        d->url += "?" + d->params.toString(QUrl::FullyEncoded);
    }

    if (d->debug) {
        qDebug() << QString("URL: %1?%2").arg(d->url).arg(d->params.toString());
    }

    QUrl urlx(d->url);
    QNetworkRequest request(urlx);
    bool internal = d->useInternalNetworkAccessManager;
    QNetworkAccessManager *manager = internal ? new QNetworkAccessManager() : d->networkAccessManager;
    QNetworkReply *reply = manager->get(request);

    //// �����ݿɶ�ȡʱ�ص� readyRead()
    //QObject::connect(reply, &QNetworkReply::readyRead, [=] {
    //    readyRead(reply->readAll());
    //});

    // �������
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        if (reply->error() == QNetworkReply::NoError && NULL != finishHandler) {
            finishHandler(reply->readAll());
        }

        // �ͷ���Դ
        reply->deleteLater();
        if (internal) {
            manager->deleteLater();
        }
    });

    // ���������
    QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] {
        if (NULL != errorHandler) {
            errorHandler(reply->errorString());
        }
    });

    // ���ؽ��ȴ���
    QObject::connect(reply, &QNetworkReply::downloadProgress, [=](qint64 arg1, qint64 arg2) {
        if (NULL != downloadProgressHandler) {
            downloadProgressHandler(arg1, arg2);
        }
    });
}

void HttpClient::upload(const QString &path,
    std::function<void(const QString &)> successHandler,
    std::function<void(const QString &)> errorHandler,
    const char *encoding) {
    if (d->debug) {
        qDebug() << QString("URL: %1").arg(d->url);
    }

    QHttpMultiPart *multiPart = new QHttpMultiPart(QHttpMultiPart::FormDataType);

    QFile *file = new QFile(path);
    file->setParent(multiPart); // we cannot delete the file now, so delete it with the multiPart

    // ����ļ���ʧ�ܣ����ͷ���Դ����
    if (!file->open(QIODevice::ReadOnly)) {
        if (NULL != errorHandler) {
            errorHandler(QString("�ļ���ʧ��: %1").arg(file->errorString()));
            multiPart->deleteLater();
            return;
        }
    }

    // �������ļ��ϴ�
    QString disposition = QString("form-data; name=\"file\"; filename=\"%1\"").arg(file->fileName());
    QHttpPart imagePart;
    imagePart.setHeader(QNetworkRequest::ContentDispositionHeader, QVariant(disposition));
    imagePart.setBodyDevice(file);
    multiPart->append(imagePart);

    bool internal = d->useInternalNetworkAccessManager;
    QNetworkRequest request(QUrl(d->url));
    QNetworkAccessManager *manager = internal ? new QNetworkAccessManager() : d->networkAccessManager;
    QNetworkReply *reply = manager->post(request, multiPart);
    multiPart->setParent(reply);

    // �������ʱһ���Զ�ȡ������Ӧ����
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        if (reply->error() == QNetworkReply::NoError && NULL != successHandler) {
            successHandler(readResponse(reply, encoding)); // �ɹ�ִ��
        }

        // �ͷ���Դ
        reply->deleteLater();
        if (internal) {
            manager->deleteLater();
        }
    });

    // ���������
    QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] {
        if (NULL != errorHandler) {
            errorHandler(reply->errorString());
        }
    });
}

// ִ������ĸ�������
void HttpClient::execute(bool posted,
    std::function<void(const QString &)> successHandler,
    std::function<void(const QString &)> errorHandler,
    const char *encoding) {
    // ����� GET ���󣬲��Ҳ�����Ϊ�գ����������Ĳ������ŵ� URL ����
    if (!posted && !d->params.isEmpty()) {
        d->url += "?" + d->params.toString(QUrl::FullyEncoded);
    }

    if (d->debug) {
        qDebug() << QString("URL: %1?%2").arg(d->url).arg(d->params.toString());
    }

    QUrl urlx(d->url);
    QNetworkRequest request(urlx);

    // �������ͷ��ӵ� request ��
    QHashIterator<QString, QString> iter(d->headers);
    while (iter.hasNext()) {
        iter.next();
        request.setRawHeader(iter.key().toUtf8(), iter.value().toUtf8());
    }

    // ���ò���֤https֤��
    QSslConfiguration config = request.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    request.setSslConfiguration(config);

    // ע��: ������ Lambda ���ʽ��ʹ�� HttpClient ����ĳ�Ա���ݣ�����������������δ����ʱ���Ѿ����������ˣ�
    // �������Ҫʹ������������ݣ�����һ���ֲ����������������ݣ�Ȼ���� Lambda ���ʽ���������ֲ�����

    // �����ʹ���ⲿ�� manager �򴴽�һ���µģ��ڷ�����ɺ���Զ�ɾ����
    bool internal = d->useInternalNetworkAccessManager;
    QNetworkAccessManager *manager = internal ? new QNetworkAccessManager() : d->networkAccessManager;
    QNetworkReply *reply = posted ? manager->post(request, d->jsonString.isEmpty()?d->params.toString(QUrl::FullyEncoded).toUtf8():d->jsonString.toUtf8()) : manager->get(request);

    // �������ʱһ���Զ�ȡ������Ӧ����
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        if (reply->error() == QNetworkReply::NoError && NULL != successHandler) {
            successHandler(readResponse(reply, encoding)); // �ɹ�ִ��
        }

        // �ͷ���Դ
        reply->deleteLater();
        if (internal) {
            manager->deleteLater();
        }
    });

    // ���������
    QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] {
		qDebug() << reply->errorString();
        if (NULL != errorHandler) {
            errorHandler(reply->errorString());
        }
    });
}

QString HttpClient::readResponse(QNetworkReply *reply, const char *encoding) {
    QTextStream in(reply);
    QString result;
    in.setCodec(encoding);

    while (!in.atEnd()) {
        result += in.readLine();
    }

    return result;
}