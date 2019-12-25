#include "hook.h"

#include <QDebug>
#include <QSettings>
#include "mhook-lib/mhook.h"
#include <process.h>
#include <qpixmap.h>
#include <mutex>


//在注册表该目录下增加新内容
#define TASKMANAGERSystem "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\System"
#define TASKMANAGERExplorer "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer"
#define DISABLEASKWINDOWPATH "Software\\Microsoft\\Terminal Server Client"

//Hook * Hook::_pInstance = 0;
//QMutex Hook::_mutex;

Hook::Hook()
{

}


Hook::~Hook()
{

}

//Hook * Hook::instance()
//{
//	if (!_pInstance)
//	{
//		QMutexLocker lock(&_mutex);
//		if (!_pInstance)
//		{
//			Hook *pInstance = new Hook();  // 修改处
//			_pInstance = pInstance;                 // 修改处
//		}
//	}
//	return _pInstance;
//}

HHOOK keyHook = NULL;
HHOOK mouseHook = NULL;

const char ALT_TABLE[] = {
	'W','D','Q','A','G'
};

//键盘钩子过程
LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//在WH_KEYBOARD_LL模式下lParam 是指向KBDLLHOOKSTRUCT类型地址
	KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *)lParam;
	//如果nCode等于HC_ACTION则处理该消息，如果小于0，则钩子子程就必须将该消息传递给 CallNextHookEx
	if (nCode == HC_ACTION)
	{
		//if ((pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000 && GetAsyncKeyState(VK_SHIFT) & 0x8000) || //"Ctrl+Shift+Esc"
		//	(pkbhs->vkCode == VK_ESCAPE && GetAsyncKeyState(VK_CONTROL) & 0x8000) || //"Ctrl+Esc"
		//	(pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN) || //"Alt+Tab"
		//	(pkbhs->vkCode == VK_ESCAPE && pkbhs->flags &LLKHF_ALTDOWN) || //"Alt+Esc"
		//	(pkbhs->vkCode == VK_LWIN || pkbhs->vkCode == VK_RWIN) ||	// "LWIN/RWIN"
		//	(pkbhs->vkCode == VK_F4 && pkbhs->flags & LLKHF_ALTDOWN)) //"Alt+F4"
		//{
		//	if ((pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP))) {
		//		emit SingleTon<Hook>::getInstance().altAndTab();
		//	}
		//	return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
		//}
		//else if (pkbhs->vkCode == 'D' && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP)) {
		//	emit SingleTon<Hook>::getInstance().altAndD();
		//	return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
		//}
		//else if (pkbhs->vkCode == 'W' && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP)) {
		//	emit SingleTon<Hook>::getInstance().altAndTab();
		//	return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
		//}
		//else if ((pkbhs->vkCode == VK_LMENU || pkbhs->vkCode == VK_RMENU) && pkbhs->flags &LLKHF_UP) {
		//	emit SingleTon<Hook>::getInstance().altUp();
		//	return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
		//}
		//else if (pkbhs->vkCode == 'Q' && (pkbhs->flags & LLKHF_ALTDOWN) /*&& (GetAsyncKeyState(VK_DELETE) & 0x8000)*/ /*&& !(pkbhs->flags &LLKHF_UP)*/) {
		//	emit SingleTon<Hook>::getInstance().altAndDel();
		//	return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
		//}
		//else if (pkbhs->vkCode == 'A' && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP)) {
		//	emit SingleTon<Hook>::getInstance().altAndA();
		//	return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
		//}
		//else if (pkbhs->vkCode == 'A' && GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		//	emit SingleTon<Hook>::getInstance().ctrlAndA();
		//	return 1;
		//}
		//else if (pkbhs->flags &LLKHF_ALTDOWN) {
		//	return 1;//返回1表示截取消息不再传递,返回0表示不作处理,消息继续传递
		//}

		/*if ((pkbhs->vkCode == VK_TAB && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP))) {
			emit SingleTon<Hook>::getInstance().altAndTab();
			pkbhs->vkCode = 0XFF;
		}
		else */
		
		//if (pkbhs->vkCode == 'W' && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP)) {
		//	//emit SingleTon<Hook>::getInstance().altAndTab();
		//	emit SingleTon<Hook>::getInstance().shortcut("Alt+W");
		//	pkbhs->vkCode = 0;
		//}
		//else if (pkbhs->vkCode == 'D' && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP)) {
		//	//emit SingleTon<Hook>::getInstance().altAndD();
		//	emit SingleTon<Hook>::getInstance().shortcut("Alt+D");
		//	pkbhs->vkCode = 0;
		//}
		//else if (pkbhs->vkCode == 'Q' && (pkbhs->flags & LLKHF_ALTDOWN) /*&& (GetAsyncKeyState(VK_DELETE) & 0x8000)*/ /*&& !(pkbhs->flags &LLKHF_UP)*/) {
		//	//emit SingleTon<Hook>::getInstance().altAndDel();
		//	emit SingleTon<Hook>::getInstance().shortcut("Alt+Q");
		//	pkbhs->vkCode = 0;
		//}
		//else if (pkbhs->vkCode == 'A' && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP)) {
		//	//emit SingleTon<Hook>::getInstance().altAndA();
		//	emit SingleTon<Hook>::getInstance().shortcut("Alt+A");
		//	pkbhs->vkCode = 0;
		//}
		//else if (pkbhs->vkCode == 'G' && ((GetAsyncKeyState(VK_RWIN) & 0x8000) || (GetAsyncKeyState(VK_LWIN) & 0x8000)) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
		//	//emit SingleTon<Hook>::getInstance().altAndG();
		//	emit SingleTon<Hook>::getInstance().shortcut("Alt+G");
		//	pkbhs->vkCode = 0;
		//}
		//else if (pkbhs->vkCode == 'A' && (GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
		//	//emit SingleTon<Hook>::getInstance().ctrlAndA();
		//	emit SingleTon<Hook>::getInstance().shortcut("Ctrl+A");
		//	pkbhs->vkCode = 0;
		//}
		//if ((pkbhs->vkCode == VK_LMENU || pkbhs->vkCode == VK_RMENU) && pkbhs->flags &LLKHF_UP) {
		//	//emit SingleTon<Hook>::getInstance().altUp();
		//	emit SingleTon<Hook>::getInstance().shortcut("AltUp");
		//}
		int i = 0;
		for (i = 0;i < sizeof(ALT_TABLE) / sizeof(ALT_TABLE[0]);i++) {
			if (pkbhs->vkCode == ALT_TABLE[i] && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP)) {
				emit SingleTon<Hook>::getInstance().shortcut("Alt+" + QString(ALT_TABLE[i]));
				pkbhs->vkCode = 0;
				break;
			}
		}
		if (i == sizeof(ALT_TABLE) / sizeof(ALT_TABLE[0])) {
			if (pkbhs->vkCode == 'A' && (GetAsyncKeyState(VK_CONTROL) & 0x8000) && (GetAsyncKeyState(VK_MENU) & 0x8000)) {
				emit SingleTon<Hook>::getInstance().shortcut("Ctrl+Alt+A");
				pkbhs->vkCode = 0;
			}
			if ((pkbhs->vkCode == VK_LMENU || pkbhs->vkCode == VK_RMENU) && pkbhs->flags &LLKHF_UP) {
				emit SingleTon<Hook>::getInstance().shortcut("AltUp");
			}
		}
	}
	return CallNextHookEx(keyHook, nCode, wParam, (LPARAM)pkbhs);
}
//鼠标钩子过程
LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam)
{

	return 1;

}


void Hook::disableRDPAskWindow(bool isDisabled)
{
	DWORD iValue = 0;
	if (isDisabled) {
		::RegSetKeyValueA(HKEY_CURRENT_USER, DISABLEASKWINDOWPATH,"AuthenticationLevelOverride", REG_DWORD,&iValue,sizeof(DWORD));
	}
	else {
		HKEY hKey = 0;
		if (ERROR_SUCCESS == ::RegOpenKeyA(HKEY_CURRENT_USER, DISABLEASKWINDOWPATH, &hKey)) {
			if (hKey != 0) {
				::RegDeleteValueA(hKey, "AuthenticationLevelOverride");
			}
		}
	}
}

QShortcut * Hook::createShortcut(QWidget *pWidget, const QKeySequence& key, bool autoRepeat) {
	QShortcut *temp = new QShortcut(pWidget);
	//设置键值，也就是设置快捷键.
	temp->setKey(key);
	//这个成员函数挺关键的，设置是否会自动反复按键.也就是说，当你一直按住键盘ctrl+r时，会一直不停的调用对应的槽函数.
	temp->setAutoRepeat(autoRepeat);
	return temp;
	
}

void Hook::init(QWidget *pWidget)
{//连接信号与槽，showSlot()是自定义的槽函数!
	connect(createShortcut(pWidget, tr("alt+w"), true), &QShortcut::activated, [&]() {
		if (!isHook)return;
		emit altAndTab();
	});
	connect(createShortcut(pWidget, tr("alt+a"), true), &QShortcut::activated, [&]() {
		if (!isHook)return;
		emit altAndA();
	});
	connect(createShortcut(pWidget, tr("alt+d"), true), &QShortcut::activated, [&]() {
		if (!isHook)return;
		emit altAndD();
	});
	connect(createShortcut(pWidget, tr("alt+q"), true), &QShortcut::activated, [&]() {
		if (!isHook)return;
		emit altAndDel();
	});
	connect(createShortcut(pWidget, tr("alt+g"), true), &QShortcut::activated, [&]() {
		if (!isHook)return;
		emit altAndG();
	});

	keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyProc, GetModuleHandle(NULL), 0);
}


//安装钩子,调用该函数即安装钩子
void Hook::hook(bool flag)
{
	isHook = flag;
	if (flag)
	{
		//这两个底层钩子,不要DLL就可以全局
		//底层键盘钩子
		keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyProc, GetModuleHandle(NULL), 0);
		//底层鼠标钩子
		//    mouseHook =SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),0);
	}
	else
	{
		UnhookWindowsHookEx(keyHook);
		//  UnhookWindowsHookEx(mouseHook);
	}
}

//添加注册屏蔽Ctrl+Alt+del
void Hook::enableTaskManager(bool flag)
{
	// 屏蔽ctrl + alt +del 需要修改注册表的值， 取得管理员权限， 关闭360等杀毒软件
	int value = flag ? 0x00000001 : 0x00000000;
	QSettings *settings = new QSettings(TASKMANAGERSystem, QSettings::NativeFormat);
	settings->setValue("DisableTaskMgr", value); //任务管理器
	settings->setValue("DisableChangePassword", value); //更改密码
	settings->setValue("DisableLockWorkstation", value); //锁定计算机
	settings->setValue("DisableSwitchUserOption", value); //切换用户

	QSettings *settings2 = new QSettings(TASKMANAGERExplorer, QSettings::NativeFormat);
	settings2->setValue("NoLogOff", value); //注销

	delete settings;
	delete settings2;
}

//屏蔽任务栏
void Hook::showTaskWindow(bool flag)
{
	if (flag)
		ShowWindow(task, SW_SHOW);
	else
	{
		task = FindWindow(L"Shell_TrayWnd", NULL);
		ShowWindow(task, SW_HIDE);
	}
}

