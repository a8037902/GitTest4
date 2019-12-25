#pragma once
#include "h5login.h"
#include "passwordh5login.h"
#include "smsh5login.h"
#include <functional>

class H5LoginContext
{
public:
    H5LoginContext(H5LoginType h5LoginType);
    ~H5LoginContext();

    void login(LoginData &loginData,
        std::function<void(QString &url, ChatServerInfo &chatSvrInfo)> successHandler,
        std::function<void()> errorHandler = NULL);

    void getChatServerInfoByDomain(QString &domain,
        std::function<void(const ChatServerInfo &chatSvrInfo)> finishHandler,
        std::function<void()> errorHandler);

private:
    H5Login *mH5Login;
};

