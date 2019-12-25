#include "powerdudewin.h"
#include "remotedesktopdialog.h"
#include <QVBoxLayout>
#include <QApplication>
#include <QDesktopWidget>
#include <QUuid>
#include <exdisp.h>

typedef interface IMsTscNonScriptable IMsTscNonScriptable;
typedef interface IMsRdpClientNonScriptable IMsRdpClientNonScriptable;

MIDL_INTERFACE("C1E6743A-41C1-4A74-832A-0DD06C1C7A0E")
IMsTscNonScriptable:public IUnknown
{
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE ResetPassword(void) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_BinaryPassword(
		/* [in] */ __RPC__in BSTR newPassword) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_BinaryPassword(
		/* [retval][out] */ __RPC__out BSTR *pBinaryPassword) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_BinarySalt(
		/* [in] */ __RPC__in BSTR newSalt) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_BinarySalt(
		/* [retval][out] */ __RPC__out BSTR *pSalt) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_ClearTextPassword(
		/* [in] */ __RPC__in BSTR newClearTextPass) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_PortablePassword(
		/* [in] */ __RPC__in BSTR newPortablePassVal) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_PortablePassword(
		/* [retval][out] */ __RPC__out BSTR *pPortablePass) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_PortableSalt(
		/* [in] */ __RPC__in BSTR newPortableSalt) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_PortableSalt(
		/* [retval][out] */ __RPC__out BSTR *pPortableSalt) = 0;
};

MIDL_INTERFACE("b3378d90-0728-45c7-8ed7-b6159fb92219")
IMsRdpClientNonScriptable : public IMsTscNonScriptable
{
public:
	virtual HRESULT STDMETHODCALLTYPE NotifyRedirectDeviceChange(
	  __RPC__in WPARAM wParam,
	  __RPC__in LPARAM lParam) = 0;
	virtual HRESULT STDMETHODCALLTYPE SendKeys(
		__RPC__in LONG         numKeys,
		__RPC__in VARIANT_BOOL *pbArrayKeyUp,
		__RPC__in LONG         *plKeyData) = 0;
	virtual HRESULT STDMETHODCALLTYPE put_UIParentWindowHandle(
		__RPC__in  HWND hwndUIParentWindowHandle
	) = 0;

	virtual HRESULT STDMETHODCALLTYPE get_UIParentWindowHandle(
		__RPC__out HWND *phwndUIParentWindowHandle
	) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_ConnectionBarText(
		/* [in] */ __RPC__in BSTR newVal) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_ConnectionBarText(
		/* [retval][out] */ __RPC__out BSTR *pConnectionBarText) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_DeviceCollection(
		/* [retval][out] */ __RPC__out void **ppDeviceCollection) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_DriveCollection(
		/* [retval][out] */ __RPC__out void **ppDeviceCollection) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_EnableCredSspSupport(
		/* [in] */ __RPC__in VARIANT_BOOL fEnableSupport) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_EnableCredSspSupport(
		/* [retval][out] */ __RPC__out VARIANT_BOOL *pfEnableSupport) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_NegotiateSecurityLayer(
		/* [in] */ __RPC__in VARIANT_BOOL fNegotiate) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_NegotiateSecurityLayer(
		/* [retval][out] */ __RPC__out VARIANT_BOOL *pfNegotiate) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_PromptForCredentials(
		/* [in] */ __RPC__in VARIANT_BOOL fPrompt) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_PromptForCredentials(
		/* [retval][out] */ __RPC__out VARIANT_BOOL *pfPrompt) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_RedirectDynamicDevices(
		/* [in] */ __RPC__in VARIANT_BOOL fRedirectDynamicDevices) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_RedirectDynamicDevices(
		/* [retval][out] */ __RPC__out VARIANT_BOOL *pfRedirectDynamicDevices) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_RedirectDynamicDrives(
		/* [in] */ __RPC__in VARIANT_BOOL fRedirectDynamicDrives) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_RedirectDynamicDrives(
		/* [retval][out] */ __RPC__out VARIANT_BOOL *pfRedirectDynamicDrives) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_ShowRedirectionWarningDialog(
		/* [in] */ __RPC__in VARIANT_BOOL fShowRdrDlg) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_ShowRedirectionWarningDialog(
		/* [retval][out] */ __RPC__out VARIANT_BOOL *pfShowRdrDlg) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_WarnAboutClipboardRedirection(
		/* [in] */ __RPC__in VARIANT_BOOL fWarn) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_WarnAboutClipboardRedirection(
		/* [retval][out] */ __RPC__out VARIANT_BOOL *pfWarn) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE put_WarnAboutSendingCredentials(
		/* [in] */ __RPC__in VARIANT_BOOL fWarn) = 0;
	virtual /* [helpcontext][helpstring][id] */ HRESULT STDMETHODCALLTYPE get_WarnAboutSendingCredentials(
		/* [retval][out] */ __RPC__out VARIANT_BOOL *pfWarn) = 0;

};

PowerdudeWin::PowerdudeWin(QWidget *parent)
	: QWebEngineView(parent)
{
	ui.setupUi(this);

	Qt::WindowFlags flags =
		(windowFlags() & Qt::WindowMinimizeButtonHint \
			& Qt::WindowCloseButtonHint);
	setWindowFlags(flags);

	QString htmlPath = QCoreApplication::applicationDirPath() + "/test.html";
	this->setUrl(QUrl("file:///"+ htmlPath));
	//this->setUrl(QUrl("https://www.baidu.com/"));

	//showFullScreen();

	//mRemoteSystemView = new QAxWidget(this);
	//QVBoxLayout *vlayout = new QVBoxLayout;
	//vlayout->setContentsMargins(0, 0, 0, 0);
	//vlayout->setSpacing(0);
	//vlayout->addWidget(mRemoteSystemView);
	//setLayout(vlayout);


	//MstscRdpInfo mstscRdpInfo;
	//mstscRdpInfo.fulladdress = "192.168.1.112";
	//mstscRdpInfo.username = "admin";// "tester";
	//mstscRdpInfo.password = "engine";
	////int index = ui.mDesktopSize->currentIndex();
	//mstscRdpInfo.desktopsize = QSize(1024, 768);
	//mRemoteSystemView->setControl("{54d38bf7-b1ef-4479-9674-1bd6ea465258}");
	//mRemoteSystemView->dynamicCall("SetDesktopWidth(int)", mstscRdpInfo.desktopsize.width());
	//mRemoteSystemView->dynamicCall("SetDesktopHeight(int)", mstscRdpInfo.desktopsize.height());
	////mRemoteSystemView->dynamicCall("SetColorDepth(int)", 32);

	//mRemoteSystemView->dynamicCall("SetRedirectDynamicDevices(bool)", true);
	//mRemoteSystemView->dynamicCall("SetRedirectDynamicDrives(bool)", true);
	//mRemoteSystemView->dynamicCall("SetConnectionBarText(const QString&)", "test");
	//mRemoteSystemView->dynamicCall("SetShowRedirectionWarningDialog(bool)", true);
	//mRemoteSystemView->dynamicCall("SetWarnAboutClipboardRedirection(bool)", true);

	//mRemoteSystemView->setFixedSize(mstscRdpInfo.desktopsize);

	//mRemoteSystemView->setProperty("Server", mstscRdpInfo.fulladdress);
	//mRemoteSystemView->setProperty("UserName", mstscRdpInfo.username);

	////QAxWidget MsRdpClientNonScriptable3;
	////MsRdpClientNonScriptable3.setControl("{b3378d90-0728-45c7-8ed7-b6159fb92219}");
	////MsRdpClientNonScriptable3.dynamicCall("SetRedirectDynamicDevices(bool)", true);

	//IMsRdpClientNonScriptable *p=0;
	//long r = mRemoteSystemView->queryInterface(QUuid("{b3378d90-0728-45c7-8ed7-b6159fb92219}"),(void**)&p);
	////p->put_ConnectionBarText((BSTR)L"test");
	////p->put_RedirectDynamicDevices(true);
	////p->put_ShowRedirectionWarningDialog(true);
	////p->put_RedirectDynamicDrives(true);
	////p->put_WarnAboutClipboardRedirection(true);



	////mRemoteSystemView->setProperty("RedirectDynamicDevices", true);


	//QAxObject *advancedSettings = mRemoteSystemView->querySubObject("AdvancedSettings8");
	//if (advancedSettings) {
	//	advancedSettings->dynamicCall("SetClearTextPassword(const QString&)", mstscRdpInfo.password);
	//	advancedSettings->dynamicCall("SetAuthenticationLevel(int)", 2);

	//	advancedSettings->dynamicCall("SetRedirectDrives(bool)", true);
	//	advancedSettings->dynamicCall("SetRedirectDevices(bool)", true);
	//	advancedSettings->dynamicCall("SetRedirectPrinters(bool)", true);
	//	advancedSettings->dynamicCall("SetRedirectPorts(bool)", true);
	//	advancedSettings->dynamicCall("SetRedirectPOSDevices(bool)", true);
	//	advancedSettings->dynamicCall("SetRedirectDirectX(bool)", true);
	//}
	


	//advancedSettings->dynamicCall("SetRedirectClipboard(bool)", false);

	//advancedSettings->dynamicCall("SetAudioRedirectionMode(int)", 2);

	//connectRemoteSystem();

}

void PowerdudeWin::connectRemoteSystem()
{
	mRemoteSystemView->dynamicCall("Connect()");
	//mRemoteSystemView->dynamicCall("RedirectNow(int)",0);
}

void PowerdudeWin::disconnectRemoteSystem()
{
	if (mRemoteSystemView != NULL)
		mRemoteSystemView->dynamicCall("Disconnect()");
}

void PowerdudeWin::closeEvent(QCloseEvent * event)
{
	//disconnectRemoteSystem();
	emit aboutToClose();
	this->close();
}
