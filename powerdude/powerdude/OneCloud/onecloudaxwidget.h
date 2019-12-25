#pragma once
#include <qwidget.h>
#include <QAxWidget>
#include <QLabel>
#include <QTimer>
#include "onecloudview.h"
namespace OneCloud {
	class OneCloudAxWidget :
		public OneCloudView
	{
		Q_OBJECT
	public:
		explicit OneCloudAxWidget(OneCloudData *pData, QWidget* parent = nullptr);
		explicit OneCloudAxWidget(QWidget* parent = nullptr);
		~OneCloudAxWidget();

		void init(QWidget* pOneCloudContainer);
		void init(QWidget* pOneCloudContainer, QRect &rect);

		void setHidden(bool hidden);
		void show();
		void hide();

		void disconnectHost();
		void connectHost();

		void setConnectInfo();

	private slots:
		//axwidget event
		void onConnected();
		void onDisconnected(int discReason);

	private:
		QAxWidget*pAxWidget;

	};
}

