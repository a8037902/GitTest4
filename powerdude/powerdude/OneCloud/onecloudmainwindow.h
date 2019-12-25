#pragma once
#include <qwidget.h>
#include <QLabel>
#include "OneCloudView.h"

namespace OneCloud {
	class OneCloudMainWindow :
		public QWidget
	{
		Q_OBJECT
	public:
		OneCloudMainWindow(OneCloudView* pOneCloudView,QWidget* parent = nullptr);
		~OneCloudMainWindow();

		void disconnectHost();
		void connectHost();

		void setKey(QString &key) {
			this->key = key;
		}
		QString& getKey() {
			return key;
		}

	signals:
		void connected(QString&);
		void disconnected(QString&);
		void toConnect(QString&);

	private slots:
		void onConnectedBtn();
		void onConnected();
		void onDisconnected();
	private:
		OneCloudView* pOneCloudView;

		QLabel*pConnectPng;
		QLabel*pConnectText;
		QWidget *pDisconnectingWidget;

		QString key;
	};
}

