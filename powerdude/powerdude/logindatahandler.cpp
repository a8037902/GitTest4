#include <QDebug>
#include <QNetworkRequest>
#include <QApplication>
#include <QProcess>
#include <QEventLoop>
#include <QTimer>
#include <QFileInfo>
#include <QTextCodec>
#include <QStringList>
#include <QMessageBox>
#include <QDesktopServices>
#include <QProgressDialog>
#include <QFuture>
#include <QtConcurrent>
#include "settings.h"
#include "logindatahandler.h"
#include "httpclient.h"
#include "websocket.h"
#include "Dongle_API.h"

#pragma comment(linker, "/NODEFAULTLIB:libc.lib")
#pragma comment(lib, "legacy_stdio_definitions.lib")

const QString SPACE_URL = "/space";
bool httpRequestAborted;
LoginDataHandler::LoginDataHandler()
{   
    m_p2pMap.insert(P2P_SMART_MODE, "all");
    m_p2pMap.insert(P2P_VPN_MODE, "openvpn");
    m_p2pMap.insert(P2P_RELAY_MODE, "relay");
}

void LoginDataHandler::getDesktopLiveUrl(QString &url, QString &msg, MstscRdpInfo &info)
{
    HttpClient(url)
        .addFormHeader()
        .addParam("appccResult", msg)
        .post([=](const QString &response) mutable {
        qDebug() << response;
    });

    return;
}

void LoginDataHandler::getP2PDesktopIpPort(MstscRdpInfo &info)
{
    QString p2pExePath = QApplication::applicationDirPath() + "/p2pagent/p2pagent.exe";

    // p2pagent.exe <client_id> <p2p_id> <p2p_mode>
    QProcess *m_process = new QProcess();
    if (QFileInfo(p2pExePath).exists())
    {
        QString workpath = QApplication::applicationDirPath() + "/p2pagent";
        SingleTon<Settings>::getInstance().beginGroup("Globa");

        P2PMode p2pMode = P2PMode(SingleTon<Settings>::getInstance().value("p2pMode", 0).toInt());
        QString clientId = WebSocket::getRandomString(8);

        SingleTon<Settings>::getInstance().endGroup();
        m_process->setWorkingDirectory(workpath);

        m_process->start(p2pExePath, QStringList() << clientId << info.p2pId << (m_p2pMap.contains(p2pMode) ? m_p2pMap[p2pMode] : "all"));
    }

    QEventLoop loop;
    connect(m_process, SIGNAL(finished(int, QProcess::ExitStatus)), &loop, SLOT(quit()));
    QTimer::singleShot(15000, &loop, SLOT(quit()));
    loop.exec();

    QTextCodec* gbkCodec = QTextCodec::codecForName("GBK");
    QString result = gbkCodec->toUnicode(m_process->readAll());

    if (!result.isNull() && !result.isEmpty()) {
        QStringList list = result.split("\n");
        bool isFound = false;
        for (QString str : list) {
            if (str.contains("p2pResult", Qt::CaseSensitive)) {
                QString result = str.section(';', 1, 1).trimmed();

                info.fulladdress = result.section(':', 0, 0).trimmed();
                info.port = result.section(':', 1, 1).trimmed().toLong();
                isFound = true;
                break;
            }
        }

        if (isFound) {
//            emit textReceived(info, QString());

        }
        else {
            // 获取不到ip和端口信息，需要显示失败
        }
    }
    else {
        // 获取不到ip和端口信息，需要显示失败
    }
}

void LoginDataHandler::downloadFile(MstscRdpInfo &info, QString &filename, QString &url)
{
    // 1. 获取设置下载路径
    SingleTon<Settings>::getInstance().beginGroup("File");
    QString fileDir = SingleTon<Settings>::getInstance().value("filePath", qApp->applicationDirPath() + "/download/").toString();
    SingleTon<Settings>::getInstance().endGroup();

    // 2. 检查设置路径是否存在，不存在则自动创建
    QDir dir(fileDir);
    if (!dir.exists()) {
        dir.mkpath(fileDir);
    }

    QString filepath = fileDir + "/" + filename;
    QFile *file = new QFile(filepath);

    if (!file->open(QIODevice::WriteOnly)) {
        QMessageBox::information(NULL, tr("HTTP"),
            tr("Unalbe to save the file %1: %2.")
            .arg(filename).arg(file->errorString()));
        delete file;
        file = 0;
        return;
    }

    QProgressDialog *m_pProgressDlg = new QProgressDialog();
    m_pProgressDlg->setWindowFlags(Qt::Dialog | Qt::CustomizeWindowHint);
    m_pProgressDlg->setWindowModality(Qt::WindowModal);
    m_pProgressDlg->setMinimumDuration(0);//dialog出现需等待的时间
    m_pProgressDlg->setLabelText(QString::fromLocal8Bit("正在下载文件：%1 ...").arg(filename));
    m_pProgressDlg->setCancelButtonText(QString::fromLocal8Bit("取消"));
    m_pProgressDlg->setMinimum(0);
    m_pProgressDlg->setMaximum(0);
    m_pProgressDlg->show();

    httpRequestAborted = false;
    connect(m_pProgressDlg, &QProgressDialog::canceled, [&]() mutable {
        httpRequestAborted = true;
    });

    HttpClient(url)
        .download
        // 读到数据
        ([=](const QByteArray &array) mutable {
            if (httpRequestAborted) {
                m_pProgressDlg->hide();
                return;
            }
            file->write(array);
        },
        // 下载完成
        ([=](const QByteArray &array) mutable {
            if (httpRequestAborted) {
                if (file) {
                    file->close();
                    file->remove();
                    file->deleteLater();
                    file = 0;
                }
                m_pProgressDlg->hide();
                return;
            }
            file->write(array);
            file->flush();
            file->close();
            file->deleteLater();

            qDebug() << "download finish";
            m_pProgressDlg->hide();
            emit textReceived(info, filepath);
        }),
        // 出错
        ([=](const QString &errString) {
            qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
            qDebug() << "Download file error: " << errString;
            QMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("接收文件错误，请检查网络."));
        }),
        // 
        ([=](__int64 & bytesReceived, __int64 & bytesTotal) mutable {
            if (httpRequestAborted)
                return;

            m_pProgressDlg->setRange(0, bytesTotal);
            m_pProgressDlg->setValue(bytesReceived);
        })
       );

    return;
}

void LoginDataHandler::getSpaceLoginUrl(QString &msg)
{
    SingleTon<Settings>::getInstance().beginGroup("Globa");
    QString spaceHost = SingleTon<Settings>::getInstance().value("space_host", "https://space.pispower.com").toString();
    SingleTon<Settings>::getInstance().endGroup();

    QString url = spaceHost + SPACE_URL;
    HttpClient(url)
        .addParam(msg)
        .addHeader("content-type", "application/json")
        .post([=](const QString &response) mutable {
        QJsonParseError jsonerror;
        QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

        if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
            QJsonObject jsonObject = document.object();
            qDebug() << QString(QJsonDocument(jsonObject).toJson());
            int code = document.object().value("code").toInt();
            if (0 == code) {
                QString key = document.object().value("key").toString();

                QString loginUrl = spaceHost + SPACE_URL + "?key=" + key;
                MstscRdpInfo info;
                info.type = 14;

                emit textReceived(info, loginUrl);
            }
        }
    });

    return;
}

bool LoginDataHandler::usbAuth(MstscRdpInfo &authData)
{
    bool result = false;
    int count;

    QMessageBox box;
    isMsgBoxClosed = false;

    QTimer::singleShot(500, &box, [&]() {
        QFuture<int> future = QtConcurrent::run(this, &LoginDataHandler::detectDongle);
        while (!future.isFinished()) {
            QApplication::processEvents(QEventLoop::AllEvents, 100);
        }

        if (0 < (count = future.result())) {
            result = dongleAuth(authData, count);
            box.close();
        }
    });

    box.setWindowTitle(QString::fromLocal8Bit("提示"));
    box.setIcon(QMessageBox::Information);
    box.setText(QString::fromLocal8Bit("该云桌面登录需要进行U盘认证，请插入U盘"));
    box.setWindowFlags(Qt::CustomizeWindowHint | Qt::WindowTitleHint);
    QPushButton *cancel = box.addButton(QString::fromLocal8Bit("取消"), QMessageBox::RejectRole);
    connect(&box, &QMessageBox::buttonClicked, [&](QAbstractButton *btn) {
        if (btn == cancel) {
            isMsgBoxClosed = true;
            box.close();
        }
    });

    box.exec();

    return result;
}

int LoginDataHandler::detectDongle()
{
    int count;
    DWORD dwRet;

    while (!isMsgBoxClosed) {
        if (DONGLE_SUCCESS == (dwRet = Dongle_Enum(NULL, &count))) {
            break;
        }
    }

    return count;
}

bool LoginDataHandler::dongleAuth(MstscRdpInfo &authData, int nCount)
{
    DWORD dwRet = 0;
    DONGLE_HANDLE hDongle = NULL;
    DONGLE_INFO *pDongleInfo = NULL;
    int nRemainCount = 0;
    char hId[20];
    char sztmp[3];
    bool bRet;

    QString serialCode = authData.usbAuthData.serialCode;
    QByteArray baSerialCode = serialCode.toLatin1();
    const char *strSerialCode = baSerialCode.data();

    QString serialPwd = authData.usbAuthData.serialPwd;
    QByteArray baSerialPwd = serialPwd.toLatin1();
    const char *strSerialPwd = baSerialPwd.data();

    pDongleInfo = (DONGLE_INFO *)malloc(nCount * sizeof(DONGLE_INFO));
    if (NULL == pDongleInfo) {
        printf("malloc memory error!\n");
        return false;
    }

    dwRet = Dongle_Enum(pDongleInfo, &nCount);//获取加密锁的相关信息
    if (dwRet != DONGLE_SUCCESS) {
        printf("Get Dongle ARM information Failed. Return value: 0x%08X\n", dwRet);
        return false;
    }

    memset(hId, 0, sizeof(hId));
    for (int k = 0; k < nCount; k++) {
        for (int i = 0; i < 8; i++) {
            printf("%02X ", pDongleInfo[k].m_HID[i]);
            sprintf(sztmp, "%02X", pDongleInfo[k].m_HID[i]);
            memcpy(&hId[i * 2], sztmp, 2);
        }
        
        if (strcmp(strSerialCode, hId)) {   // 硬件ID不一致
            if (NULL != pDongleInfo) {
                free(pDongleInfo);
                pDongleInfo = NULL;
            }

            return false;
        }
    }

    dwRet = Dongle_Open(&hDongle, 0);//打开第1把锁

    dwRet = Dongle_VerifyPIN(hDongle, FLAG_ADMINPIN, (char *)strSerialPwd, &nRemainCount); //验证开发商PIN码
    if (dwRet != DONGLE_SUCCESS) {
        dwRet = Dongle_Close(hDongle);//关闭加密锁
        if (NULL != pDongleInfo) {
            free(pDongleInfo);
            pDongleInfo = NULL;
        }

        return false;
    }

    dwRet = Dongle_Close(hDongle);//关闭加密锁

    if (NULL != pDongleInfo) {
        free(pDongleInfo);
        pDongleInfo = NULL;
    }

    return true;
}


void LoginDataHandler::handleDesktopGroupData(QString &msg)
{
    if (msg.isEmpty()) {
        return;
    }

    QJsonParseError error;
    QJsonDocument jsonDocument = QJsonDocument::fromJson(msg.toUtf8().data(), &error);
    if (jsonDocument.isNull()) {
        qDebug() << error.errorString();
        return;
    }

    QJsonObject jsonObject = jsonDocument.object();
    QJsonArray jsonArrayData = jsonObject.value("data").toArray();

    MstscRdpInfo desktopGroupInfo;
    desktopGroupInfo.desktopGroupData.bizId = jsonObject.value("groupbizid").toString();
    desktopGroupInfo.desktopGroupData.name = jsonObject.value("groupname").toString();
    desktopGroupInfo.desktopGroupData.count = jsonArrayData.count();
    desktopGroupInfo.name = jsonObject.value("groupname").toString();
    desktopGroupInfo.type = DESKTOP_GROUP_TYPE;
    
    for (int i = 0; i < jsonArrayData.count(); i++) {
        MstscRdpInfo info;

        info.type = 2;   // 桌面
        info.desktopType = RDPDestkopType(jsonArrayData.at(i).toObject().value("type").toInt());

        info.name = jsonArrayData.at(i).toObject().value("name").toString();
        info.fulladdress = jsonArrayData.at(i).toObject().value("ip").toString();
        info.port = jsonArrayData.at(i).toObject().value("port").toString().toInt();
        info.username = jsonArrayData.at(i).toObject().value("username").toString();
        info.password = jsonArrayData.at(i).toObject().value("password").toString();
        info.domain = jsonArrayData.at(i).toObject().value("domain").toString();
        info.p2pId = jsonArrayData.at(i).toObject().value("p2p_id").toString();

        if (!info.p2pId.isEmpty()) {
            desktopGroupInfo.desktopKeyList.insert(i, info.p2pId);
        } else {
            desktopGroupInfo.desktopKeyList.insert(i, QString("%1@%2:%3").arg(info.username).arg(info.fulladdress).arg(info.port));
        }

        emit textReceived(info, msg);
    }

    emit textReceived(desktopGroupInfo, msg);
}