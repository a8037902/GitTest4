#include "libScreenShot.h"
#include "ScreenShotDialog.h"

static QSimpleScreenShot* _instance = 0;

QSimpleScreenShot::QSimpleScreenShot(QObject *parent)
	:QObject(parent),
	m_pScreenShot(0)
{

}

QSimpleScreenShot* QSimpleScreenShot::instance()
{
	if (_instance == 0) {
		_instance = new QSimpleScreenShot();
	}
	return _instance;
}

void QSimpleScreenShot::screenShot(QWidget* pWidget)
{
	if (m_pScreenShot == 0) {
		ScreenShotDialog* screenShot = new ScreenShotDialog(pWidget);
		m_pScreenShot = screenShot;
		screenShot->initScreenShot();
		screenShot->exec();
		delete screenShot;
		m_pScreenShot = 0;
	}
}

bool QSimpleScreenShot::isScreenShot()
{
	return m_pScreenShot != 0;
}

void QSimpleScreenShot::onScreenShotClosed(void* pParam)
{

}