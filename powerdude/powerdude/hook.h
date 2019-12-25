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
	Hook();                             //˽�л����캯����ʹ���಻�����ⲿ���ɶ���
	Hook(const Hook&);               //˽�л��������캯����ʹ���಻��Ϊ������������ʼ��
	Hook& operator= (const Hook&);   //˽�л���ֵ��������ʹ���಻��Ϊ�����������ֵ

	friend class SingleTon<Hook>;        //singletonģ����Ϊ�������Ե�࣬������new����
public:
	~Hook();

	//static Hook * instance();
	void init(QWidget *pWidget);
	QShortcut * createShortcut(QWidget *pWidget, const QKeySequence& key, bool autoRepeat = false);

	void hook(bool flag); //���ܼ�
	void showTaskWindow(bool flag); //��ʾ������
	void enableTaskManager(bool flag); //���������
	void disableRDPAskWindow(bool isDisabled = true);//��ֹRDP��ʾѯ�ʴ���
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

