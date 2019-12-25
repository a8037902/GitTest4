#pragma once
#include <qsettings.h>
#include <QtXml>
#include "singleton.h"
class Settings :
	public QSettings
{
	Q_OBJECT
private:
	Settings();                             //˽�л����캯����ʹ���಻�����ⲿ���ɶ���
	Settings(const Settings&);               //˽�л��������캯����ʹ���಻��Ϊ������������ʼ��
	Settings& operator= (const Settings&);   //˽�л���ֵ��������ʹ���಻��Ϊ�����������ֵ

	friend class SingleTon<Settings>;        //singletonģ����Ϊ�������Ե�࣬������new����

public:
	~Settings();

	void setOneValue(const QString &group, const QString &key, const QVariant &value);
	QVariant oneValue(const QString &group, const QString &key, const QVariant &defaultValue = QVariant());

	void WriteXml();
	void ReadXml();
	void AddXml();
};

