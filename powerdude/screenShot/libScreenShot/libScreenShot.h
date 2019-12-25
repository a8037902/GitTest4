#pragma once

#include "libscreenshot_global.h"
#include <QObject>

class LIBSCREENSHOT_EXPORT QSimpleScreenShot:QObject
{
	Q_OBJECT
private:
	QSimpleScreenShot(QObject *parent = 0);

public:
	static QSimpleScreenShot* instance();
	void screenShot(QWidget* pWidget);
	bool isScreenShot();
private:
	void* m_pScreenShot;

private slots:
	void onScreenShotClosed(void* );
};
