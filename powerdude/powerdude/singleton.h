#pragma once
#include <QMutex>
#include <QReadWriteLock>
#include <QAtomicPointer>
template <class T>
class SingleTon {
public:
	static T &getInstance(void)
	{
#ifdef Q_ATOMIC_POINTER_TEST_AND_SET_IS_ALWAYS_NATIVE
		if (!QAtomicPointer<T>::isTestAndSetNative())//运行时进行检测
		{
			qDebug() << "Error: don's support TestAndSetNative!!!!!!";
		}
#endif
		//双重检测加锁
		if (instance.testAndSetOrdered(0, 0)) {
			QMutexLocker locker(&mutex);
			instance.testAndSetOrdered(0, new T);
		}
		return *instance;
	}
protected:
	SingleTon() {}
private://下与上相同
	QReadWriteLock internalMutex;
	static QMutex mutex;
	static QAtomicPointer<T> instance;
};

//初始化静态变量
template <class T>
QMutex SingleTon<T>::mutex;

template <class T>
QAtomicPointer<T> SingleTon<T>::instance = 0;
