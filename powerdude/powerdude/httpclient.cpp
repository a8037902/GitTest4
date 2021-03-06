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

    QString url; // 请求的 URL
    QUrlQuery params; // 请求的参数
    QString jsonString; // Json格式请求参数
    QHash<QString, QString> headers; // 请求的头
    QNetworkAccessManager *networkAccessManager;
    bool useInternalNetworkAccessManager; // 是否使用内部的 QNetworkAccessManager
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

// 传入 debug 为 true 则使用 debug 模式，请求执行时输出请求的 URL 和参数等
HttpClient &HttpClient::debug(bool debug) {
    d->debug = debug;
    return *this;
}

// 增加参数
HttpClient &HttpClient::addParam(const QString &name, const QString &value) {
    d->params.addQueryItem(name, value);
    return *this;
}

// 增加Json请求参数
HttpClient &HttpClient::addParam(const QString &jsonString) {
    d->jsonString = jsonString;
    return *this;
}

// 增加访问头
HttpClient &HttpClient::addHeader(const QString &header, const QString &value) {
    d->headers[header] = value;
    return *this;
}

HttpClient &HttpClient::addFormHeader() {
    return addHeader("content-type", "application/x-www-form-urlencoded");
}

// 执行 GET 请求
void HttpClient::get(std::function<void(const QString &)> successHandler,
    std::function<void(const QString &)> errorHandler,
    const char *encoding) {
    execute(false, successHandler, errorHandler, encoding);
}

// 执行 POST 请求
void HttpClient::post(std::function<void(const QString &)> successHandler,
    std::function<void(const QString &)> errorHandler,
    const char *encoding) {
    execute(true, successHandler, errorHandler, encoding);
}

// 使用 GET 进行下载，当有数据可读取时回调 readyRead(), 大多数情况下应该在 readyRead() 里把数据保存到文件
void HttpClient::download(std::function<void(const QByteArray &)> readyRead,
    std::function<void(const QByteArray &)> finishHandler,
    std::function<void(const QString &)> errorHandler,
    std::function<void(__int64 & bytesReceived, __int64 & bytesTotal)> downloadProgressHandler) {
    // 如果是 GET 请求，并且参数不为空，则编码请求的参数，放到 URL 后面
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

    //// 有数据可读取时回调 readyRead()
    //QObject::connect(reply, &QNetworkReply::readyRead, [=] {
    //    readyRead(reply->readAll());
    //});

    // 请求结束
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        if (reply->error() == QNetworkReply::NoError && NULL != finishHandler) {
            finishHandler(reply->readAll());
        }

        // 释放资源
        reply->deleteLater();
        if (internal) {
            manager->deleteLater();
        }
    });

    // 请求错误处理
    QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] {
        if (NULL != errorHandler) {
            errorHandler(reply->errorString());
        }
    });

    // 下载进度处理
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

    // 如果文件打开失败，则释放资源返回
    if (!file->open(QIODevice::ReadOnly)) {
        if (NULL != errorHandler) {
            errorHandler(QString("文件打开失败: %1").arg(file->errorString()));
            multiPart->deleteLater();
            return;
        }
    }

    // 表明是文件上传
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

    // 请求结束时一次性读取所有响应数据
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        if (reply->error() == QNetworkReply::NoError && NULL != successHandler) {
            successHandler(readResponse(reply, encoding)); // 成功执行
        }

        // 释放资源
        reply->deleteLater();
        if (internal) {
            manager->deleteLater();
        }
    });

    // 请求错误处理
    QObject::connect(reply, static_cast<void (QNetworkReply::*)(QNetworkReply::NetworkError)>(&QNetworkReply::error), [=] {
        if (NULL != errorHandler) {
            errorHandler(reply->errorString());
        }
    });
}

// 执行请求的辅助函数
void HttpClient::execute(bool posted,
    std::function<void(const QString &)> successHandler,
    std::function<void(const QString &)> errorHandler,
    const char *encoding) {
    // 如果是 GET 请求，并且参数不为空，则编码请求的参数，放到 URL 后面
    if (!posted && !d->params.isEmpty()) {
        d->url += "?" + d->params.toString(QUrl::FullyEncoded);
    }

    if (d->debug) {
        qDebug() << QString("URL: %1?%2").arg(d->url).arg(d->params.toString());
    }

    QUrl urlx(d->url);
    QNetworkRequest request(urlx);

    // 把请求的头添加到 request 中
    QHashIterator<QString, QString> iter(d->headers);
    while (iter.hasNext()) {
        iter.next();
        request.setRawHeader(iter.key().toUtf8(), iter.value().toUtf8());
    }

    // 设置不验证https证书
    QSslConfiguration config = request.sslConfiguration();
    config.setPeerVerifyMode(QSslSocket::VerifyNone);
    config.setProtocol(QSsl::TlsV1SslV3);
    request.setSslConfiguration(config);

    // 注意: 不能在 Lambda 表达式里使用 HttpClient 对象的成员数据，因其可能在网络访问未结束时就已经被析构掉了，
    // 所以如果要使用它的相关数据，定义一个局部变量来保存其数据，然后在 Lambda 表达式里访问这个局部变量

    // 如果不使用外部的 manager 则创建一个新的，在访问完成后会自动删除掉
    bool internal = d->useInternalNetworkAccessManager;
    QNetworkAccessManager *manager = internal ? new QNetworkAccessManager() : d->networkAccessManager;
    QNetworkReply *reply = posted ? manager->post(request, d->jsonString.isEmpty()?d->params.toString(QUrl::FullyEncoded).toUtf8():d->jsonString.toUtf8()) : manager->get(request);

    // 请求结束时一次性读取所有响应数据
    QObject::connect(reply, &QNetworkReply::finished, [=] {
        if (reply->error() == QNetworkReply::NoError && NULL != successHandler) {
            successHandler(readResponse(reply, encoding)); // 成功执行
        }

        // 释放资源
        reply->deleteLater();
        if (internal) {
            manager->deleteLater();
        }
    });

    // 请求错误处理
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