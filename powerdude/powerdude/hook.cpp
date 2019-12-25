#include "hook.h"

#include <QDebug>
#include <QSettings>
#include "mhook-lib/mhook.h"
#include <process.h>
#include <qpixmap.h>
#include <mutex>


//��ע����Ŀ¼������������
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
//			Hook *pInstance = new Hook();  // �޸Ĵ�
//			_pInstance = pInstance;                 // �޸Ĵ�
//		}
//	}
//	return _pInstance;
//}

HHOOK keyHook = NULL;
HHOOK mouseHook = NULL;

const char ALT_TABLE[] = {
	'W','D','Q','A','G'
};

//���̹��ӹ���
LRESULT CALLBACK keyProc(int nCode, WPARAM wParam, LPARAM lParam)
{
	//��WH_KEYBOARD_LLģʽ��lParam ��ָ��KBDLLHOOKSTRUCT���͵�ַ
	KBDLLHOOKSTRUCT *pkbhs = (KBDLLHOOKSTRUCT *)lParam;
	//���nCode����HC_ACTION�������Ϣ�����С��0�������ӳ̾ͱ��뽫����Ϣ���ݸ� CallNextHookEx
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
		//	return 1;//����1��ʾ��ȡ��Ϣ���ٴ���,����0��ʾ��������,��Ϣ��������
		//}
		//else if (pkbhs->vkCode == 'D' && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP)) {
		//	emit SingleTon<Hook>::getInstance().altAndD();
		//	return 1;//����1��ʾ��ȡ��Ϣ���ٴ���,����0��ʾ��������,��Ϣ��������
		//}
		//else if (pkbhs->vkCode == 'W' && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP)) {
		//	emit SingleTon<Hook>::getInstance().altAndTab();
		//	return 1;//����1��ʾ��ȡ��Ϣ���ٴ���,����0��ʾ��������,��Ϣ��������
		//}
		//else if ((pkbhs->vkCode == VK_LMENU || pkbhs->vkCode == VK_RMENU) && pkbhs->flags &LLKHF_UP) {
		//	emit SingleTon<Hook>::getInstance().altUp();
		//	return 1;//����1��ʾ��ȡ��Ϣ���ٴ���,����0��ʾ��������,��Ϣ��������
		//}
		//else if (pkbhs->vkCode == 'Q' && (pkbhs->flags & LLKHF_ALTDOWN) /*&& (GetAsyncKeyState(VK_DELETE) & 0x8000)*/ /*&& !(pkbhs->flags &LLKHF_UP)*/) {
		//	emit SingleTon<Hook>::getInstance().altAndDel();
		//	return 1;//����1��ʾ��ȡ��Ϣ���ٴ���,����0��ʾ��������,��Ϣ��������
		//}
		//else if (pkbhs->vkCode == 'A' && pkbhs->flags & LLKHF_ALTDOWN && !(pkbhs->flags &LLKHF_UP)) {
		//	emit SingleTon<Hook>::getInstance().altAndA();
		//	return 1;//����1��ʾ��ȡ��Ϣ���ٴ���,����0��ʾ��������,��Ϣ��������
		//}
		//else if (pkbhs->vkCode == 'A' && GetAsyncKeyState(VK_CONTROL) & 0x8000) {
		//	emit SingleTon<Hook>::getInstance().ctrlAndA();
		//	return 1;
		//}
		//else if (pkbhs->flags &LLKHF_ALTDOWN) {
		//	return 1;//����1��ʾ��ȡ��Ϣ���ٴ���,����0��ʾ��������,��Ϣ��������
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
//��깳�ӹ���
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
	//���ü�ֵ��Ҳ�������ÿ�ݼ�.
	temp->setKey(key);
	//�����Ա����ͦ�ؼ��ģ������Ƿ���Զ���������.Ҳ����˵������һֱ��ס����ctrl+rʱ����һֱ��ͣ�ĵ��ö�Ӧ�Ĳۺ���.
	temp->setAutoRepeat(autoRepeat);
	return temp;
	
}

void Hook::init(QWidget *pWidget)
{//�����ź���ۣ�showSlot()���Զ���Ĳۺ���!
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


//��װ����,���øú�������װ����
void Hook::hook(bool flag)
{
	isHook = flag;
	if (flag)
	{
		//�������ײ㹳��,��ҪDLL�Ϳ���ȫ��
		//�ײ���̹���
		keyHook = SetWindowsHookEx(WH_KEYBOARD_LL, keyProc, GetModuleHandle(NULL), 0);
		//�ײ���깳��
		//    mouseHook =SetWindowsHookEx( WH_MOUSE_LL,mouseProc,GetModuleHandle(NULL),0);
	}
	else
	{
		UnhookWindowsHookEx(keyHook);
		//  UnhookWindowsHookEx(mouseHook);
	}
}

//���ע������Ctrl+Alt+del
void Hook::enableTaskManager(bool flag)
{
	// ����ctrl + alt +del ��Ҫ�޸�ע����ֵ�� ȡ�ù���ԱȨ�ޣ� �ر�360��ɱ�����
	int value = flag ? 0x00000001 : 0x00000000;
	QSettings *settings = new QSettings(TASKMANAGERSystem, QSettings::NativeFormat);
	settings->setValue("DisableTaskMgr", value); //���������
	settings->setValue("DisableChangePassword", value); //��������
	settings->setValue("DisableLockWorkstation", value); //���������
	settings->setValue("DisableSwitchUserOption", value); //�л��û�

	QSettings *settings2 = new QSettings(TASKMANAGERExplorer, QSettings::NativeFormat);
	settings2->setValue("NoLogOff", value); //ע��

	delete settings;
	delete settings2;
}

//����������
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

