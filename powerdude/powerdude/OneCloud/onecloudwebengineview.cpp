#include "onecloudwebengineview.h"
#include <qDebug>
#include <QWebEngineProfile>

namespace OneCloud {
	OneCloudWebEngineView::OneCloudWebEngineView(OneCloudData *pData, QWidget *parent)
		: OneCloudView(pData, parent), pWebEngineView(NULL)
	{

	}

	OneCloudWebEngineView::OneCloudWebEngineView(QWidget *parent)
		: OneCloudView(parent), pWebEngineView(NULL)
	{
		
	}


	OneCloudWebEngineView::~OneCloudWebEngineView()
	{

	}
	void OneCloudWebEngineView::init(QWidget * pOneCloudContainer)
	{
		pWebEngineView = new QWebEngineView(pOneCloudContainer);
		//pWebEngineView->setObjectName(QString::fromUtf8("webEngineView"));
		pWebEngineView->setGeometry(QRect(QPoint(0, 0), pOneCloudContainer->size()));
		setDownloadPath(getOneCloudData("downloadPath").toString());

		//connect(pWebEngineView, SIGNAL(loadFinished()), this, SLOT(onLoadFinished()));
	}
	void OneCloudWebEngineView::init(QWidget * pOneCloudContainer, QRect & rect)
	{
		pWebEngineView = new QWebEngineView(pOneCloudContainer);
		//pWebEngineView->setObjectName(QString::fromUtf8("webEngineView"));
		pWebEngineView->setGeometry(rect);

		connect(pWebEngineView, SIGNAL(loadFinished()), this, SLOT(onLoadFinished()));
		connect(pWebEngineView, SIGNAL(loadProgress(int)), this, SLOT(onLoadProgress(int)));
	}
	void OneCloudWebEngineView::setHidden(bool hidden)
	{
	}
	void OneCloudWebEngineView::show()
	{
		//pWebEngineView->show();
		pWebEngineView->raise();
	}

	void OneCloudWebEngineView::hide()
	{
		//pWebEngineView->hide();
		pWebEngineView->lower();
	}

	void OneCloudWebEngineView::disconnectHost()
	{
		//pWebEngineView->setUrl(QString(""));
		emit disconnected();
	}

	void OneCloudWebEngineView::connectHost()
	{
		//pWebEngineView->setUrl(getOneCloudData("url").toString());
		pWebEngineView->load(getOneCloudData("url").toString());
	}

	void OneCloudWebEngineView::onLoadProgress(int progress)
	{
		qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
		if (progress >= 100) {
			emit connected();
		}
	}

	void OneCloudWebEngineView::setDownloadPath(const QString & path)
	{
		QWebEnginePage* pWebEnginePage = pWebEngineView->page();

		QWebEngineProfile *pWebEngineProfile = pWebEnginePage->profile();//new QWebEngineProfile();
		pWebEngineProfile->setDownloadPath(path);
	}

	void OneCloudWebEngineView::onLoadFinished()
	{
		qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
		//emit connected();
	}
}