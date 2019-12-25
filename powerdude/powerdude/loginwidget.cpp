#include "loginwidget.h"
#include "setdialog.h"
#include <QPainter>
#include <QMessageBox>
#include <QBitmap>
#include <QApplication>
#include <QDesktopWidget>
#include "settings.h"
#include "hook.h"
#include "httpclient.h"
#include "onecloudmessagebox.h"

const unsigned MININPUTSIZE = 8;

LoginWidget::LoginWidget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::LoginWidget())
    , closeEn(false)
    , login(false)
    , loginType(0)
    , h5LoginType(H5LoginType::PasswordLogin)
    , mSmsH5Login(new SmsH5Login())
{
	ui->setupUi(this);
	m_iMaskShow = 0;

	//QPalette pe;
	//pe.setColor(QPalette::Background, Qt::white);
	//QPixmap bgImage(":/powerdudemain/Resources/loginBG.png");
	//QBitmap mask = bgImage.createMaskFromColor(QColor(0, 0, 0), Qt::MaskInColor);
	//bgImage.setMask(mask);
	//bgImage.scaled(this->size());//, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
	//pe.setBrush(QPalette::Background, QBrush(bgImage/*.scaled(this->size())*/));
	//this->setPalette(pe);
	//this->setAutoFillBackground(true);

	//QRegExp regExp("[0-9][0-9.][0-9.][0-9.][0-9.][0-9.][0-9.][0-9.][0-9.][0-9.][0-9.][0-9.][0-9.][0-9.][0-9.]");
	QRegExp regExp("\\b(?:(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\.){3}(?:25[0-5]|2[0-4][0-9]|[01]?[0-9][0-9]?)\\b");
	ui->ipEdit->setValidator(new QRegExpValidator(regExp, this));

	regExp = QRegExp("[0-9][0-9][0-9][0-9][0-9]");
	ui->protEdit->setValidator(new QRegExpValidator(regExp, this));

	regExp = QRegExp("^[A-Za-z0-9`~!@#$%^&()_+-=]+$");
	ui->userNameEdit->setValidator(new QRegExpValidator(regExp, this));
	ui->realmNameEdit->setValidator(new QRegExpValidator(regExp, this));

    regExp = QRegExp("^[0-9]*$");
    ui->desktopNumEdit_2->setValidator(new QRegExpValidator(regExp, this));

    regExp = QRegExp("^[0-9]{8}$");
    ui->desktopPwdEdit->setValidator(new QRegExpValidator(regExp, this));

    regExp = QRegExp("^1([358][0-9]|4[579]|66|7[0135678]|9[89])[0-9]{8}$");
    ui->mobileEdit->setValidator(new QRegExpValidator(regExp, this));

	ui->hideLabel->hide();
	ui->hideLabel->raise();
	connect(ui->label_4, &WebLabel::enter, [&]() {
		ui->hideLabel->show();
	});
	connect(ui->label_4, &WebLabel::leave, [&]() {
		ui->hideLabel->hide();
	});
	connect(ui->label_4, &WebLabel::mouseMove, [&](QCursor&cursor) {
		//qDebug() << "Mouse " << this->parentWidget().x() << " " << this->parentWidget().y();
		ui->hideLabel->move(cursor.pos().x()-this->parentWidget()->x()+10, cursor.pos().y() - this->parentWidget()->y());
	});

	//this->setStyleSheet("QWidget{ background - color:gray;border-radius:15px }");

	//ui->widget->setGeometry(QRect(this->size().width() / 2 - 20, this->size().height() / 2 - 20, 40, 40));
	//ui->widget->hide();
	ui->widget_2->setGeometry(QRect(this->size().width() / 2 - 110, this->size().height() / 2 - 110, 220, 220));
	//ui->label->setGeometry(QRect(this->size().width() / 2 - 210, this->size().height() / 2 - 140, 420, 20));
	ui->appRemindLabel->setGeometry(QRect(this->size().width() / 2 - 100, this->size().height() / 2 + 120, 200, 20));
	ui->label->setGeometry(QRect(this->size().width() / 2 - 210, this->size().height() / 2 + 145, 420, 20));

	//ui->updateBtn->setGeometry(QRect(this->size().width() / 2 - 60, this->size().height() / 2 + 70, 120, 30));

	ui->verticalWidget->hide();
    ui->h5chatWidget->hide();
	ui->qrLoginBtn->hide();
    ui->ycBtn->hide();
    ui->soBtn->hide();
    ui->chatserverwidget->hide();

    mSmsH5Login->getChatServerInfoList([=](const QList<ChatServerInfo> &chatSvrInfoList) {
        qDebug() << chatSvrInfoList[0].chatServerName;
        loadChatServerList(chatSvrInfoList);
    },
        [=]() {

    });
	//ui->appRemindLabel->hide();
	//ui->updateBtn->hide();

	ui->checkBox->setChecked(SingleTon<Settings>::getInstance().oneValue("Globa", "remember", false).toBool());

    SingleTon<Settings>::getInstance().beginGroup("Globa");
    m_deskAuthHost = SingleTon<Settings>::getInstance().value("deskauth_host", "https://deskauth.pispower.com").toString();
    SingleTon<Settings>::getInstance().endGroup();

	connect(ui->label_4, &WebLabel::clicked, [&]() {
		qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
		emit clicked();
	});
	char strBuffer[MAX_PATH];


	connect(ui->ipLogin, &QAbstractButton::clicked, [&]() {
		loginType = 0;
		loginTypeChange(loginType);
	});
	connect(ui->numLogin, &QAbstractButton::clicked, [&]() {
		loginType = 1;
		loginTypeChange(loginType);
	});
    connect(ui->pwdLogin, &QAbstractButton::clicked, [&]() {
        h5LoginType = H5LoginType::PasswordLogin;
        h5LoginTypeChange(h5LoginType);
    });
    connect(ui->smsLogin, &QAbstractButton::clicked, [&]() {
        h5LoginType = H5LoginType::SmsLogin;
        h5LoginTypeChange(h5LoginType);
    });
    connect(ui->upBtn, &QAbstractButton::clicked, [&]() {
       /* QIcon icon(":/powerdudemain/Resources/yc@2x.png");
        int length = mSmsH5Login->chatServerInfoList().size();
        ui->upBtn->setIcon(icon);
*/
    });
    connect(ui->soBtn, &QAbstractButton::clicked, [&]() {
        //OneCloudMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("so so so"));
    });

    connect(ui->pushButton, &QAbstractButton::clicked, [&]() {
        if (!validInputInfo()) {
            return;
        }

        QString ip = ui->ipEdit->text();
        int port = ui->protEdit->text().toInt();
        QString username = ui->userNameEdit->text();
        QString password = ui->passwordEdit->text();
        QString domain = ui->realmNameEdit->text();

        QJsonObject rootObj;
        rootObj.insert("protocol", "rdp");

        QJsonObject desktopObj;
        desktopObj.insert("ip", ip);
        desktopObj.insert("port", port);
        desktopObj.insert("username", username);
        desktopObj.insert("password", password);
        desktopObj.insert("domain", domain);

        rootObj.insert("desktop", desktopObj);

        QString jsonString = QString(QJsonDocument(rootObj).toJson());

        // https://desktopauth.pispower.com/register/dynamic_token_desktop
        QString url = QString("%1%2").arg(m_deskAuthHost).arg("/register/dynamic_token_desktop");

        HttpClient(url)
            .addParam(jsonString)
            .addHeader("content-type", "application/json")
            .post([=](const QString &response) mutable {
            qDebug() << response;

            QJsonParseError jsonerror;
            QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

            if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
                QJsonObject jsonObject = document.object();
                QJsonObject jsonObjectData = jsonObject.value("data").toObject();

                int code = document.object().value("code").toInt();
                if (code == 0) {
                    QString num = jsonObjectData.value("num").toString();
                    QString seed = jsonObjectData.value("seed").toString();

                    SingleTon<Settings>::getInstance().beginGroup("Globa");
                    QString appccHost = SingleTon<Settings>::getInstance().value("website", "https//appcc.pispower.com").toString();
                    SingleTon<Settings>::getInstance().endGroup();

                    QString t = QString::number(QDateTime::currentDateTime().toMSecsSinceEpoch());

                    QString url = QString("%1%2%3%4%5%6%7").arg(appccHost).arg("/s?T=27&n=").arg(num).arg("&t=").arg(t).arg("&k=").arg(seed);

                    QDialog dlg(this);
                    dlg.setGeometry(QRect(QApplication::desktop()->screenGeometry().width() / 2 - 110, QApplication::desktop()->screenGeometry().height() / 2 - 110, 220, 220));
                    WebLabel *pLabel = new WebLabel(&dlg);
                    pLabel->onShowUrl(url);
                    dlg.exec();
                }
                else {
                    QString msg = document.object().value("message").toString();
                    OneCloudMessageBox::warning(this, QStringLiteral("警告"), msg);
                }
            }
        },
        [=](const QString &resp) {
            QString str = resp;
            OneCloudMessageBox::warning(this, QStringLiteral("警告"), str);
        });
    });
}


LoginWidget::~LoginWidget()
{
}

void LoginWidget::stopMovie()
{

}

void LoginWidget::onShowUrl(QString &url)
{
	//ui->updateBtn->hide();
	ui->label_4->onShowUrl(url);
}

void LoginWidget::onShowMovie()
{
	ui->label_4->onShowMovie();
}

void LoginWidget::onDisconnected()
{
	ui->label_4->onDisconnected();
}

void LoginWidget::onLoginInfo(MstscRdpInfo & info)
{
	ui->ipEdit->setText(info.fulladdress);
	if(info.port>0)ui->protEdit->setText(QString::number(info.port));// 11003;
	ui->userNameEdit->setText(info.username);// "QApublic3";
	ui->passwordEdit->setText(info.password);
	ui->realmNameEdit->setText(info.domain);
}

void LoginWidget::on_ipLoginBtn_clicked()
{
	ui->widget_2->hide();
    ui->h5chatWidget->hide();
    ui->chatserverwidget->hide();
	ui->label->hide();
	ui->appRemindLabel->hide();
	ui->verticalWidget->show();
	loginTypeChange(loginType);

	ui->ipLoginBtn->hide();
    ui->h5LoginBtn->hide();
	ui->qrLoginBtn->show();
}

void LoginWidget::on_qrLoginBtn_clicked()
{
	//ui->widget->show();
	ui->widget_2->show();
	ui->label->show();
	ui->appRemindLabel->show();

	ui->verticalWidget->hide();
    ui->h5chatWidget->hide();
    ui->chatserverwidget->hide();

	ui->ipLoginBtn->show();
	if (m_iMaskShow == 1) {
		ui->h5LoginBtn->hide();
	}
	else{
		ui->h5LoginBtn->show();
	}
	ui->qrLoginBtn->hide();
}

void LoginWidget::on_h5LoginBtn_clicked()
{
    ui->widget_2->hide();
    ui->verticalWidget->hide();

    ui->appRemindLabel->hide();
    ui->h5chatWidget->show();
    ui->chatserverwidget->show();
    h5LoginTypeChange(h5LoginType);

    ui->ipLoginBtn->hide();
    ui->h5LoginBtn->hide();
    ui->qrLoginBtn->show();
}

void LoginWidget::on_loginBtn_clicked()
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;

	if (loginType==0 && validInputInfo())
	{
		MstscRdpInfo mstscRdpInfo;
		mstscRdpInfo.type = 0;
		mstscRdpInfo.desktopType = RDPDestkopType::NormalDesktop;
		mstscRdpInfo.fulladdress = ui->ipEdit->text();
		mstscRdpInfo.port = ui->protEdit->text().toInt();// 11003;
		mstscRdpInfo.username = ui->userNameEdit->text();// "QApublic3";
		mstscRdpInfo.password = ui->passwordEdit->text();
		mstscRdpInfo.domain = ui->realmNameEdit->text();

		emit localLogin(mstscRdpInfo);
	}
    
    if (loginType == 1 && validDynamicDesktopInfo()) {
        // login dynamic desktop
        qDebug() << "login";

        QString desktopNum = ui->desktopNumEdit_2->text();
        QString desktopCmd = ui->desktopPwdEdit->text();

        // https://deskauth.pispower.com/auth/dynamic_token_desktop/token
        QString url = QString("%1%2%3%4").arg(m_deskAuthHost)
            .arg("/auth/dynamic_token_desktop/").arg(desktopNum).arg(desktopCmd);

        HttpClient(url)
            .get([=](const QString &response) mutable {
            qDebug() << response;

            QJsonParseError jsonerror;
            QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

            if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
                QJsonObject jsonObject = document.object();
                QJsonObject jsonObjectData = jsonObject.value("data").toObject();

                int code = document.object().value("code").toInt();
                if (code == 0) {
                    QJsonObject jsonObjectDesktop = jsonObjectData.value("desktop").toObject();

                    MstscRdpInfo mstscRdpInfo;
                    mstscRdpInfo.type = 0;
                    mstscRdpInfo.fulladdress = jsonObjectDesktop.value("ip").toString();
                    mstscRdpInfo.port = jsonObjectDesktop.value("port").toInt();
                    mstscRdpInfo.username = jsonObjectDesktop.value("username").toString();
                    mstscRdpInfo.password = jsonObjectDesktop.value("password").toString();
                    mstscRdpInfo.domain = jsonObjectDesktop.value("domain").toString();

                    emit localLogin(mstscRdpInfo);
                }
                else if (code == 404) {
					OneCloudMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("桌面号码不存在，请重新输入。"));
                }
                else if (code == 403) {
					OneCloudMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("桌面口令验证失败，请检查口令是否失效。"));
                }
                else if (code == 7) {
					OneCloudMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("请求服务器出错，请联系管理员。"));
                }
            }
        });
    }
}

void LoginWidget::on_closeBtn_clicked()
{
	//qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	emit close();
}

void LoginWidget::on_minBtn_clicked()
{
	emit toMin();
}

void LoginWidget::on_setBtn_clicked()
{
	//SetDialog *dlgSet = new SetDialog(this);
	//SetDialog dlgSet(this);
	//dlgSet.exec();
	////if (ret == QDialog::Accepted)
	//{
	//	SingleTon<Settings>::getInstance().beginGroup("Globa");
	//	bool en = SingleTon<Settings>::getInstance().value("hotKeyEn", false).toBool();
	//	SingleTon<Settings>::getInstance().endGroup();

	//	if (en) {
	//		SingleTon<Hook>::getInstance().hook(true);
	//		QObject::connect(&SingleTon<Hook>::getInstance(), SIGNAL(altAndTab()), this, SLOT(onAltAndTab()));
	//		QObject::connect(&SingleTon<Hook>::getInstance(), SIGNAL(altAndD()), this, SLOT(onAltAndD()));
	//		QObject::connect(&SingleTon<Hook>::getInstance(), SIGNAL(altUp()), this, SLOT(onAltUp()));
	//	}
	//	else {
	//		SingleTon<Hook>::getInstance().hook(false);
	//		QObject::disconnect(&SingleTon<Hook>::getInstance(), SIGNAL(altAndTab()), this, SLOT(onAltAndTab()));
	//		QObject::disconnect(&SingleTon<Hook>::getInstance(), SIGNAL(altAndD()), this, SLOT(onAltAndD()));
	//		QObject::disconnect(&SingleTon<Hook>::getInstance(), SIGNAL(altUp()), this, SLOT(onAltUp()));
	//	}
	//}
	//delete dlgSet;
}

void LoginWidget::on_checkBox_clicked(bool check)
{
	SingleTon<Settings>::getInstance().setOneValue("Globa","remember", check);
}

void LoginWidget::on_updateBtn_clicked()
{
	//ui->updateBtn->hide();
	ui->label_4->onShowMovie();
	emit updateBtnClicked();
}

void LoginWidget::paintEvent(QPaintEvent *e)
{
	QPainter painter(this);
//
//	const qreal radius = 10;
//	QRectF rect = QRect(0, 0, this->width(), this->height());
//
//	QPainterPath path;
//
//	path.moveTo(rect.bottomRight() - QPointF(0, radius));
//	path.lineTo(rect.topRight() + QPointF(0, radius));
//	path.arcTo(QRectF(QPointF(rect.topRight() - QPointF(radius * 2, 0)), QSize(radius * 2, radius * 2)), 0, 90);
//	path.lineTo(rect.topLeft() + QPointF(radius, 0));
//	path.arcTo(QRectF(QPointF(rect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
//	path.lineTo(rect.bottomLeft() - QPointF(0, radius));
//	path.arcTo(QRectF(QPointF(rect.bottomLeft() - QPointF(0, radius * 2)), QSize(radius * 2, radius * 2)), 180, 90);
//	path.lineTo(rect.bottomLeft() + QPointF(radius, 0));
//	path.arcTo(QRectF(QPointF(rect.bottomRight() - QPointF(radius * 2, radius * 2)), QSize(radius * 2, radius * 2)), 270, 90);
//
//	painter.fillPath(path, QColor(250, 255, 250));
//
	QPixmap pixmap(":/powerdudemain/Resources/loginBG.png");
	QBitmap mask = pixmap.createMaskFromColor(QColor(0, 0, 0), Qt::MaskInColor);
	pixmap.setMask(mask);
	painter.drawPixmap(pixmap.rect(), pixmap);

	QWidget::paintEvent(e);
}

void LoginWidget::hideEvent(QHideEvent * event)
{
	ui->label_4->stopMovie();
}

void LoginWidget::loginTypeChange(int type)
{
	if (type == 0) {
		ui->ipLogin->setStyleSheet("background-color: rgb(3, 144, 223);");
		ui->numLogin->setStyleSheet("background-color: rgb(250, 250, 250);");
		ui->desktopNumEdit_2->hide();
		ui->desktopPwdEdit->hide();
		ui->checkBox->show();
		ui->ipEdit->show();
		ui->protEdit->show();
		ui->realmNameEdit->show();
		ui->userNameEdit->show();
		ui->passwordEdit->show();
		ui->pushButton->show();
		ui->verticalWidget->setGeometry(200, 80, 200, 270);
	}
	else if (type == 1) {
		ui->numLogin->setStyleSheet("background-color: rgb(3, 144, 223);");
		ui->ipLogin->setStyleSheet("background-color: rgb(250, 250, 250);");
		ui->desktopNumEdit_2->show();
		ui->desktopPwdEdit->show();
		ui->checkBox->hide();
		ui->ipEdit->hide();
		ui->protEdit->hide();
		ui->realmNameEdit->hide();
		ui->userNameEdit->hide();
		ui->passwordEdit->hide();
		ui->pushButton->hide();
		ui->verticalWidget->setGeometry(200, 80, 200, 150);
	}
}

bool LoginWidget::validInputInfo()
{

	/*QRegExp rx("^([1]?/d/d?|2[0-1]/d|22[0-3])/.([1]?/d/d?|2[0-4]/d|25[0-5])/.([1]?/d/d?|2[0-4]/d|25[0-5])/.([1]?/d/d?|2[0-4]/d|25[0-5])$");
	if (!rx.exactMatch(ui->ipEdit->text()))
	{
		QMessageBox::information(this, QStringLiteral("错误"), QStringLiteral("ip地址错误"));
		return false;
	}*/

	if (ui->ipEdit->text().isEmpty())
	{
		//QMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("ip地址为空"));
		OneCloudMessageBox::information(this, QStringLiteral("ip地址为空"));
		//execMessageBox("Input warning", "Please input correct connect addr!!!");
		return false;
	}
	if (ui->protEdit->text().isEmpty())
	{
		OneCloudMessageBox::information(this, QStringLiteral("端口为空"));
		//execMessageBox("Input warning", "Please input correct connect addr!!!");
		return false;
	}
	else if (ui->userNameEdit->text().isEmpty())
	{
		OneCloudMessageBox::information(this, QStringLiteral("用户名为空"));
		//execMessageBox("Input warning", "Please input username!!!");
		return false;
	}
	else if (ui->passwordEdit->text().isEmpty())
	{
		OneCloudMessageBox::information(this, QStringLiteral("密码为空"));
		return false;
	}

	return true;
}

bool LoginWidget::validDynamicDesktopInfo()
{
    if (ui->desktopNumEdit_2->text().isEmpty()) {
		OneCloudMessageBox::information(this, QStringLiteral("桌面号码不能为空！"));
        return false;
    } 
    else if (ui->desktopPwdEdit->text().isEmpty()) {
		OneCloudMessageBox::information(this, QStringLiteral("桌面口令不能为空"));
        return false;
    }

    QString currText = ui->desktopPwdEdit->text();
    int currCount = currText.count();

    if (currCount < MININPUTSIZE)
    {
		OneCloudMessageBox::information(this, QStringLiteral("桌面口令格式有误，口令格式为8位数字！"));
        return false;
    }

    return true;
}

void LoginWidget::h5LoginTypeChange(H5LoginType h5LoginType)
{
    if (h5LoginType == H5LoginType::PasswordLogin) {
        ui->pwdLogin->setStyleSheet("background-color: rgb(3, 144, 223);");
        ui->smsLogin->setStyleSheet("background-color: rgb(250, 250, 250);");
        ui->codeEdit->hide();
        ui->sendSmsBtn->hide();
        ui->mobileEdit->hide();
        ui->accountEdit->show();
        ui->pwdEdit->show();
        ui->h5chatWidget->setGeometry(200, 80, 200, 270);
    }
    else if (h5LoginType == H5LoginType::SmsLogin) {
        ui->smsLogin->setStyleSheet("background-color: rgb(3, 144, 223);");
        ui->pwdLogin->setStyleSheet("background-color: rgb(250, 250, 250);");
        ui->desktopNumEdit_2->show();
        ui->pwdEdit->hide();
        ui->accountEdit->hide();
        ui->mobileEdit->show();
        ui->codeEdit->show();
        ui->sendSmsBtn->show();
        ui->h5chatWidget->setGeometry(200, 80, 200, 270);
    }
}

void LoginWidget::on_h5chatLoginBtn_clicked()
{
    ui->h5chatLoginBtn->setEnabled(false);
    if (validH5LoginInfo(h5LoginType)) {
        H5LoginContext *context = new H5LoginContext(h5LoginType);
        QString domainName = ui->domainEdit->text();

        context->getChatServerInfoByDomain(domainName, [=](const ChatServerInfo &chatSvrInfo) mutable {
            LoginData loginData;
            loginData.platformType = PlatFormType::H5;

            if (h5LoginType == H5LoginType::PasswordLogin) {
                loginData.account = ui->accountEdit->text();
                loginData.password = ui->pwdEdit->text();
            }
            if (h5LoginType == H5LoginType::SmsLogin) {
                loginData.mobile = ui->mobileEdit->text();
                loginData.smsCode = ui->codeEdit->text();
            }

            context->login(loginData, [this](QString &url, ChatServerInfo &chatSvrInfo) mutable {
                QTimer::singleShot(2000, this, [&]() {
                    resetH5LoginUI();
                });
                emit this->h5chatLogin(url, chatSvrInfo);
            }, [=]() {ui->h5chatLoginBtn->setEnabled(true); });  // 登陆错误，恢复按键
        }, [=]() {ui->h5chatLoginBtn->setEnabled(true); });  // 获取域名信息错误，恢复按键
    }
    else {
        ui->h5chatLoginBtn->setEnabled(true);
    }
}

void LoginWidget::on_sendSmsBtn_clicked()
{
    ui->sendSmsBtn->setEnabled(false);

    if (!validSmsSendInfo()) {
        ui->sendSmsBtn->setEnabled(true);
        return;
    }

    QString mobile = ui->mobileEdit->text();
    QString domainName = ui->domainEdit->text();
    mSmsH5Login->getChatServerInfoByDomain(domainName, [=](const ChatServerInfo &chatSvrInfo) mutable {
        mSmsH5Login->sendSms(mobile,
            [=]() {
            ui->sendSmsBtn->setEnabled(true);
        },
            [=]() {
            ui->sendSmsBtn->setEnabled(true);
        }
        );
    }, [=]() {ui->sendSmsBtn->setEnabled(true); });
}

bool LoginWidget::validH5LoginInfo(H5LoginType h5LoginType)
{
    if (ui->domainEdit->text().isEmpty()) {
        OneCloudMessageBox::information(this, QStringLiteral("小慧域名不能为空！"));
        return false;
    }
    if (h5LoginType == H5LoginType::PasswordLogin)
    {
        if (ui->accountEdit->text().isEmpty()) {
            OneCloudMessageBox::information(this, QStringLiteral("账号不能为空！"));
            return false;
        }
        if (ui->pwdEdit->text().isEmpty()) {
            OneCloudMessageBox::information(this, QStringLiteral("密码不能为空！"));
            return false;
        }
    }
    if (h5LoginType == H5LoginType::SmsLogin)
    {
        if (ui->mobileEdit->text().isEmpty()) {
            OneCloudMessageBox::information(this, QStringLiteral("手机号不能为空！"));
            return false;
        }
        if (ui->codeEdit->text().isEmpty()) {
            OneCloudMessageBox::information(this, QStringLiteral("验证码不能为空！"));
            return false;
        }
    }

    return true;
}

bool LoginWidget::validSmsSendInfo()
{
    if (ui->domainEdit->text().isEmpty()) {
		OneCloudMessageBox::information(this, QStringLiteral("小慧域名不能为空！"));
        return false;
    }

    if (ui->mobileEdit->text().isEmpty()) {
		OneCloudMessageBox::information(this, QStringLiteral("手机号不能为空！"));
        return false;
    }

    return true;
}

void LoginWidget::resetH5LoginUI()
{
    ui->domainEdit->clear();
    ui->mobileEdit->clear();
    ui->pwdEdit->clear();
    ui->codeEdit->clear();
    ui->accountEdit->clear();

    ui->sendSmsBtn->setEnabled(true);
    ui->h5chatLoginBtn->setEnabled(true);
}

void LoginWidget::loadChatServerList(const QList<ChatServerInfo> &chatSvrInfoList)
{
    QList<QPushButton> pushBtnList;
    QRect rect = ui->chatserverwidget->geometry();
    QRect extendBtnRect = ui->upBtn->geometry();

    for (int i = 0; i < chatSvrInfoList.size(); i++) {
        ChatServerInfo info = chatSvrInfoList[i];
        QPushButton *btn = new QPushButton();
        
        HttpClient(info.recommendImageUrl)
            .download([=](const QByteArray &data){
            qDebug() << "read data";
        },
            [=](const QByteArray &data) {
            QPixmap pix;
            pix.loadFromData(data);

            QIcon icon(pix);
            btn->setIcon(icon);
            btn->resize(rect.width()/3, rect.height() - extendBtnRect.height());
            btn->setIconSize(btn->rect().size());

            btn->move(rect.width() / 3 * i, extendBtnRect.height());
            btn->setParent(ui->chatserverwidget);
            btn->setStyleSheet("background: transparent");

            connect(btn, &QPushButton::clicked, this, [=]() {
                ui->domainEdit->setText(info.chatServerName);
            });

            btn->show();
        });
    }
}