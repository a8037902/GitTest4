#pragma once
#include "h5login.h"
#include "chatsvrinfo.h"

class PasswordH5Login : public H5Login
{
    Q_OBJECT
public:
    ~PasswordH5Login() {};

    void Login(LoginData &loginData,
        std::function<void(QString &url, ChatServerInfo &chatSvrInfo)> successHandler,
        std::function<void()> errorHandler = NULL);

signals:
    void h5chatLogin(QString &url, ChatServerInfo &chatSvrInfo);
};

