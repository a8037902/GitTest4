#include "powerdude.h"
#include <QApplication>
#include <QDesktopWidget>
#include <QSettings>
#include <QMessageBox>

Powerdude::Powerdude(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);

	connect(ui.loginBtn, &QPushButton::clicked, this, &Powerdude::connectRemoteSystem);

	Qt::WindowFlags flags =
		(windowFlags() & Qt::WindowMinimizeButtonHint \
			& Qt::WindowCloseButtonHint);
	setWindowFlags(flags);
	setFixedSize(this->width(), this->height());

	QRegExp rex("((2[0-4]\\d|25[0-5]|[01]?\\d\\d?)\\.){3}(2[0-4]\\d|25[0-5]|[01]?\\d\\d?)");
	QRegExpValidator *pReg = new QRegExpValidator(rex, this);
	ui.ipEdit->setValidator(pReg);

	readConnectIniInfo();
}

QString Powerdude::GetVersion(void) {
	return Powerdude::version;
}

void Powerdude::connectRemoteSystem()
{
	if (validInputInfo())
	{
		setConnectInfo();
		RemoteDesktopDialog *dialog = new RemoteDesktopDialog(mMstscRdpInfo);
		QRect screen = QApplication::desktop()->screenGeometry();
		dialog->setAttribute(Qt::WA_DeleteOnClose, true);
		//连接远程系统
		dialog->connectRemoteSystem();
		dialog->show();
		dialog->move(screen.center() - dialog->rect().center());
		connect(dialog, SIGNAL(aboutToClose()), this, SLOT(onAboutToClose()));
		dialog->raise();
		dialog->activateWindow();

		saveCurrentConnectInfo();

		hide();
	}
	
}

bool Powerdude::validInputInfo()
{
	if (ui.ipEdit->text().isEmpty())
	{
		execMessageBox("Input warning", "Please input correct connect addr!!!");
		return false;
	}
	else if (ui.userNameEdit->text().isEmpty())
	{
		execMessageBox("Input warning", "Please input username!!!");
		return false;
	}
	/*else if (ui.passwordEdit->text().isEmpty())
	{
		execMessageBox("Input warning", "Please input password!!!");
		return false;
	}*/

	return true;
}

void Powerdude::setConnectInfo()
{
	//mMstscRdpInfo.drivestoredirect = getSelectedLocalDrivers();
	mMstscRdpInfo.fulladdress = ui.ipEdit->text();
	mMstscRdpInfo.username = ui.userNameEdit->text();
	mMstscRdpInfo.password = ui.passwordEdit->text();
	//int index = ui.mDesktopSize->currentIndex();
	mMstscRdpInfo.desktopsize = QSize(1280, 768);
}
void Powerdude::saveCurrentConnectInfo()
{
	QString settingsPath = qApp->applicationDirPath() + "\\remote-settings.ini";
	QSettings settings(settingsPath, QSettings::IniFormat);
	settings.beginGroup("RemoteDesktop");
	settings.setValue("ConnectAddr", mMstscRdpInfo.fulladdress);
	settings.setValue("Username", mMstscRdpInfo.username);
	settings.setValue("Password", mMstscRdpInfo.password);
	//settings.setValue("JoinSelectedDrivers", mMstscRdpInfo.drivestoredirect);
	settings.endGroup();
}

void Powerdude::readConnectIniInfo()
{
	QString settingsPath = qApp->applicationDirPath() + "\\remote-settings.ini";
	QSettings settings(settingsPath, QSettings::IniFormat);
	settings.beginGroup("RemoteDesktop");
	//mJoinSelectedDrivers = settings.value("JoinSelectedDrivers").toString();
	ui.ipEdit->setText(settings.value("ConnectAddr").toString());
	ui.userNameEdit->setText(settings.value("Username").toString());
	ui.passwordEdit->setText(settings.value("Password").toString());

	settings.endGroup();
}
void Powerdude::onAboutToClose()
{
	show();
}

void Powerdude::execMessageBox(const QString & title, const QString & text)
{
	QMessageBox msgbox;
	msgbox.setWindowTitle(QObject::tr(title.toStdString().c_str()));
	msgbox.setText(QObject::tr(text.toStdString().c_str()));
	msgbox.exec();
}