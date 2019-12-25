#pragma once
#include "h5login.h"
#include "chatsvrinfo.h"

class SmsH5Login : public H5Login
{
    Q_OBJECT
public:
    ~SmsH5Login() {};

    void Login(LoginData &loginData,
        std::function<void(QString &url, ChatServerInfo &chatSvrInfo)> successHandler,
        std::function<void()> errorHandler = NULL);

    void sendSms(QString &mobile,
        std::function<void()> successHandler = NULL,
        std::function<void()> errorHandler = NULL);

signals:
    void h5chatLogin(QString &url, ChatServerInfo &chatSvrInfo);
};