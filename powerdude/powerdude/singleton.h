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
		if (!QAtomicPointer<T>::isTestAndSetNative())//����ʱ���м��
		{
			qDebug() << "Error: don's support TestAndSetNative!!!!!!";
		}
#endif
		//˫�ؼ�����
		if (instance.testAndSetOrdered(0, 0)) {
			QMutexLocker locker(&mutex);
			instance.testAndSetOrdered(0, new T);
		}
		return *instance;
	}
protected:
	SingleTon() {}
private://��������ͬ
	QReadWriteLock internalMutex;
	static QMutex mutex;
	static QAtomicPointer<T> instance;
};

//��ʼ����̬����
template <class T>
QMutex SingleTon<T>::mutex;

template <class T>
QAtomicPointer<T> SingleTon<T>::instance = 0;
