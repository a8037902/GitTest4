#include "h5logincontext.h"

H5LoginContext::H5LoginContext(H5LoginType h5LoginType)
{
    switch (h5LoginType) {
    case H5LoginType::PasswordLogin:
        mH5Login = new PasswordH5Login();
        break;

    case H5LoginType::SmsLogin:
        mH5Login = new SmsH5Login();
        break;

    default:
        break;
    }

}

H5LoginContext::~H5LoginContext()
{
    if (mH5Login) delete mH5Login;
}

void H5LoginContext::login(LoginData &loginData, 
    std::function<void(QString &url, ChatServerInfo &chatSvrInfo)> successHandler,
    std::function<void()> errorHandler)
{
    if (mH5Login) {
        mH5Login->Login(loginData, successHandler, errorHandler);
    }
}

void H5LoginContext::getChatServerInfoByDomain(QString &domain,
    std::function<void(const ChatServerInfo &chatSvrInfo)> finishHandler,
    std::function<void()> errorHandler)
{
    if (mH5Login) {
        mH5Login->getChatServerInfoByDomain(domain, finishHandler, errorHandler);
    }
}