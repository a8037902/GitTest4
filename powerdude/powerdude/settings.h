#pragma once
#include <qsettings.h>
#include <QtXml>
#include "singleton.h"
class Settings :
	public QSettings
{
	Q_OBJECT
private:
	Settings();                             //私有化构造函数，使本类不能再外部生成对象
	Settings(const Settings&);               //私有化拷贝构造函数，使本类不能为其它本类对象初始化
	Settings& operator= (const Settings&);   //私有化赋值操作符，使本类不能为其它本类对象赋值

	friend class SingleTon<Settings>;        //singleton模板类为本类的友缘类，它才能new对象

public:
	~Settings();

	void setOneValue(const QString &group, const QString &key, const QVariant &value);
	QVariant oneValue(const QString &group, const QString &key, const QVariant &defaultValue = QVariant());

	void WriteXml();
	void ReadXml();
	void AddXml();
};

