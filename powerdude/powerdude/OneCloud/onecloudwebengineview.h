#pragma once
#include <qwidget.h>
#include <QWebEngineView>
#include <QLabel>
#include <QTimer>
#include "onecloudview.h"
namespace OneCloud {
	class OneCloudWebEngineView :
		public OneCloudView
	{
		Q_OBJECT
	public:
		explicit OneCloudWebEngineView(OneCloudData *pData, QWidget* parent = nullptr);
		explicit OneCloudWebEngineView(QWidget* parent = nullptr);
		~OneCloudWebEngineView();

		void init(QWidget* pOneCloudContainer);
		void init(QWidget* pOneCloudContainer,QRect &rect);

		void setDownloadPath(const QString &path);

		void setHidden(bool hidden);
		void show();
		void hide();

		void disconnectHost();
		void connectHost();
	signals:


	protected:

	private slots:
		//axwidget event
		void onLoadFinished();
		void onLoadProgress(int);

	private:
		QWebEngineView*pWebEngineView;

	};
}