#include "remotedesktopdialog.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>


RemoteDesktopDialog::RemoteDesktopDialog(const MstscRdpInfo &mstscRdpInfo, QWidget *parent)
	: QDialog(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags =
		(windowFlags() & Qt::WindowMinimizeButtonHint \
			& Qt::WindowCloseButtonHint);
	setWindowFlags(flags);

	//showFullScreen();

	mRemoteSystemView = new QAxWidget(this);
	QVBoxLayout *vlayout = new QVBoxLayout;
	vlayout->setContentsMargins(0, 0, 0, 0);
	vlayout->setSpacing(0);
	vlayout->addWidget(mRemoteSystemView);
	setLayout(vlayout);


	//mRemoteSystemView->setControl("{A3BC03A0-041D-42E3-AD22-882B7865C9C5}");	
	mRemoteSystemView->setControl("{54d38bf7-b1ef-4479-9674-1bd6ea465258}");
	mRemoteSystemView->dynamicCall("SetDesktopWidth(int)", mstscRdpInfo.desktopsize.width()/*QApplication::desktop()->screenGeometry().width()*/);
	mRemoteSystemView->dynamicCall("SetDesktopHeight(int)", mstscRdpInfo.desktopsize.height()/*QApplication::desktop()->screenGeometry().height()*/);
	//mRemoteSystemView->dynamicCall("SetColorDepth(int)", 32);

	mRemoteSystemView->dynamicCall("SetRedirectDynamicDevices(bool)", true);
	mRemoteSystemView->dynamicCall("SetRedirectDynamicDrives(bool)", true);

	mRemoteSystemView->setFixedSize(mstscRdpInfo.desktopsize);

	mRemoteSystemView->setProperty("Server", mstscRdpInfo.fulladdress);
	mRemoteSystemView->setProperty("UserName", mstscRdpInfo.username);

	
	
	//mRemoteSystemView->setProperty("RedirectDynamicDevices", true);
	

	QAxObject *advancedSettings = mRemoteSystemView->querySubObject("AdvancedSettings8");
	if (advancedSettings) {
		advancedSettings->dynamicCall("SetClearTextPassword(const QString&)", mstscRdpInfo.password);
		advancedSettings->dynamicCall("SetAuthenticationLevel(int)", 2);
	}
	advancedSettings->dynamicCall("SetRedirectDrives(bool)", true);
	advancedSettings->dynamicCall("SetRedirectDevices(bool)", true);
	advancedSettings->dynamicCall("SetRedirectPrinters(bool)", true);
	advancedSettings->dynamicCall("SetRedirectPorts(bool)", true);
	advancedSettings->dynamicCall("SetRedirectPOSDevices(bool)", true);

	
	//advancedSettings->dynamicCall("SetRedirectClipboard(bool)", false);
	
	//advancedSettings->dynamicCall("SetAudioRedirectionMode(int)", 2);
	


}

void RemoteDesktopDialog::connectRemoteSystem()
{
	mRemoteSystemView->dynamicCall("Connect()");
	//mRemoteSystemView->dynamicCall("RedirectNow(int)",0);
}

void RemoteDesktopDialog::disconnectRemoteSystem()
{
	if (mRemoteSystemView != NULL)
		mRemoteSystemView->dynamicCall("Disconnect()");
}

void RemoteDesktopDialog::closeEvent(QCloseEvent * event)
{
	//disconnectRemoteSystem();
	emit aboutToClose();
	this->close();
}
