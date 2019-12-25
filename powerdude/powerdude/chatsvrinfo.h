#pragma once
#include <QString>
#include <QSize>

class ChatServerInfo
{
public:
    QString token;
    QString chatServerApi;
    QString chatServerId;
    QString chatServerName;
    QString xiaohui;

    QString openfireChatletCache;
    QString openfireBosh;

    QString recommendImageUrl;
   
    ChatServerInfo() {}
};