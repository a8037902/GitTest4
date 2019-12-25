#pragma once
#include <qobject.h>
#include <QMutex>
#include <Windows.h>
#include "SingleTon.h"
#include <QShortcut>

class Hook :
	public QObject/*, public SingleTon<Hook>*/
{
	Q_OBJECT
private:
	Hook();                             //私有化构造函数，使本类不能再外部生成对象
	Hook(const Hook&);               //私有化拷贝构造函数，使本类不能为其它本类对象初始化
	Hook& operator= (const Hook&);   //私有化赋值操作符，使本类不能为其它本类对象赋值

	friend class SingleTon<Hook>;        //singleton模板类为本类的友缘类，它才能new对象
public:
	~Hook();

	//static Hook * instance();
	void init(QWidget *pWidget);
	QShortcut * createShortcut(QWidget *pWidget, const QKeySequence& key, bool autoRepeat = false);

	void hook(bool flag); //功能键
	void showTaskWindow(bool flag); //显示任务栏
	void enableTaskManager(bool flag); //任务管理器
	void disableRDPAskWindow(bool isDisabled = true);//禁止RDP显示询问窗口
signals:
	void shortcut(const QString &key);
	void altAndTab();
	void altAndD();
	void altAndA();
	void altUp();
	void altAndDel();
	void ctrlAndA();
	void altAndG();
private:
	HWND task;
	bool isHook;
	//static Hook *_pInstance;
	//static QMutex _mutex;
};

