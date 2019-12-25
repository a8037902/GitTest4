#pragma once
#include <QObject>
#include "chatsvrinfo.h"
#include <functional>

enum H5LoginType {
    PasswordLogin = 0,
    SmsLogin = 1
};

enum PlatFormType {
    Android = 1,
    iOS = 2,
    WindowsPhone = 3,
    H5 = 4,
    SDK = 5
};

struct LoginData {
    QString account;
    QString mobile;
    QString password;

    QString smsCode;
    int platformType;
};

const QString LOGIN_FORWARD_URL = QStringLiteral("/login/goChatHtml5/chatserverinfo");

class H5Login : public QObject{
    Q_OBJECT
public:
    H5Login(QObject *parent = nullptr);

    ChatServerInfo chatServerInfo() const;
    void setChatServerInfo(const ChatServerInfo &chatServerInfo);

    QList<ChatServerInfo> chatServerInfoList() const;
    void setChatServerInfoList(const QList<ChatServerInfo> &chatServerInfoList);

    virtual void Login(LoginData &loginData, 
        std::function<void(QString &url, ChatServerInfo &chatSvrInfo)> successHandler,
        std::function<void()> errorHandler = NULL) = 0;

    QString constructLoginUrl(ChatServerInfo &chatServerInfo, QString &token);

    void getChatServerInfoByDomain(QString &domain, 
        std::function<void(const ChatServerInfo &chatSvrInfo)> finishHandler = NULL,
        std::function<void()> errorHandler = NULL);

    void getChatServerInfoList(std::function<void(const QList<ChatServerInfo> &chatSvrInfoList)> finishHandler = NULL,
        std::function<void()> errorHandler = NULL);

private:
    ChatServerInfo mChatServerInfo;
    QList<ChatServerInfo> mChatServerInfoList;
};

inline ChatServerInfo H5Login::chatServerInfo() const
{
    return mChatServerInfo;
}

inline  QList<ChatServerInfo> H5Login::chatServerInfoList() const
{
    return mChatServerInfoList;
}