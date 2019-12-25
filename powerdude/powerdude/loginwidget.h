#pragma once
#include <qwidget.h>
#include <QTimer>
#include "chatsvrinfo.h"
#include "ui_loginwidget.h"
#include "h5logincontext.h"
#include "passwordh5login.h"


class LoginWidget :
	public QWidget
{
	Q_OBJECT
public:
	explicit LoginWidget(QWidget *parent = Q_NULLPTR);
	~LoginWidget();

	bool validInputInfo();
	void setCloseEn(bool en) {
		closeEn = en;
		if (en) {
			ui->setBtn->hide();
			ui->minBtn->hide();
		}
	}
	bool remember() {
		return ui->checkBox->isChecked();
	}

	void setRemember(bool check) {
		ui->checkBox->setChecked(check);
	}

	void clearPassword() {
		ui->passwordEdit->clear();
	}

	void setLogin(bool login) {
		if (this->login != login) {
			this->login = login;
			if (login) {
				//on_ipLoginBtn_clicked();
				//ui->qrLoginBtn->hide();
				ui->label->setText(QStringLiteral("可扫码登录：云桌面、网站账号、云桌面文件、同屏桌面、SSH账号、VNC账号"));
				ui->appRemindLabel->setText(QStringLiteral("推荐使用小慧app扫码无关联登入"));
				ui->h5LoginBtn->hide();
			}
			else {
				//ui->qrLoginBtn->show();
				ui->label->setText("");
				ui->appRemindLabel->setText(QStringLiteral("推荐使用小慧app扫码登入"));
				ui->h5LoginBtn->show();
			}
		}
	}

	void setMaskShow(int iMaskShow) {
		m_iMaskShow = iMaskShow;
		if (m_iMaskShow == 1) {//login add 
			ui->setBtn->hide();
			ui->minBtn->hide();
		}
		else if(m_iMaskShow == 2){//not login add
			ui->setBtn->hide();
			ui->minBtn->hide();
		}
		else {//login panel show normal
			ui->setBtn->show();
			ui->minBtn->show();
		}
	}

	void stopMovie();

signals:
	void localLogin(MstscRdpInfo &info);
	void close();
	void toMin();
	void clicked();
	void updateBtnClicked();

    void h5chatLogin(QString &url, ChatServerInfo &chatSvrInfo);

public slots:
	void onShowUrl(QString &url);
	void onShowMovie();
	void onDisconnected();
	void onLoginInfo(MstscRdpInfo &info);

	void on_ipLoginBtn_clicked();
	void on_qrLoginBtn_clicked();
    void on_h5LoginBtn_clicked();
	void on_loginBtn_clicked();
	void on_closeBtn_clicked();
	void on_minBtn_clicked();
	void on_setBtn_clicked();
	void on_checkBox_clicked(bool);
	void on_updateBtn_clicked();
    void on_h5chatLoginBtn_clicked();

    void on_sendSmsBtn_clicked();

protected:
	virtual void paintEvent(QPaintEvent*);
	virtual void hideEvent(QHideEvent *event);

	void loginTypeChange(int type);

    void h5LoginTypeChange(H5LoginType h5LoginType);

private:
	Ui::LoginWidget* ui;

	bool closeEn;
	bool login;
	int m_iMaskShow;

	int loginType;
	QString m_deskAuthHost;
	bool validDynamicDesktopInfo();
    bool validH5LoginInfo(H5LoginType h5LoginType);
    bool validSmsSendInfo();
    void resetH5LoginUI();
    void loadChatServerList(const QList<ChatServerInfo> &chatSvrInfoList);

    H5LoginType h5LoginType;
    SmsH5Login *mSmsH5Login;
};
