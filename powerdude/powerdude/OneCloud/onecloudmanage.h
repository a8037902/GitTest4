#pragma once
#include <qwidget.h>
#include <QMap>
#include "OneCloudMainWindow.h"

namespace OneCloud {
	class OneCloudManage :
		public QObject
	{
		Q_OBJECT
	public:
		OneCloudManage();
		~OneCloudManage();

		void setOneCloudContainer(QWidget* pOneCloudContainer) {
			this->pOneCloudContainer = pOneCloudContainer;
		}

		void openOneCloudMainWindow(QString & key, QString & name, OneCloudView* pOneCloudView, QIcon &icon);
		void openOneCloudMainWindow(QString & key);
		void closeOneCloudMainWindow(QString & key);
		void closeOneCloudMainWindow(OneCloudMainWindow*pOneCloudMainWindow);
		void closeFrontWindow();
		void clear();

		void disconnectHost(QString& key);
		void connectHost(QString& key);
		
		void frontWidgetShow(OneCloudMainWindow*pOneCloudMainWindow);
		int getNextWidget() {
			int index = 0;
			if (pFrontOneCloudMainWindow != NULL) {
				index = (oneCloudList.indexOf(pFrontOneCloudMainWindow) + +oneCloudList.size() - 1) % oneCloudList.size();
			}
			return index;
		}
		void changeWidget(int index) {
			if (oneCloudList.size() > index) {
				frontWidgetShow(oneCloudList[index]);
			}
		}

		bool contains(QString& key);

	signals:
		void connected(QString&);
		void disconnected(QString&);
		void toConnect(QString&);
		void toClose(QString&);
		void toOpen(QString&);

	private slots:
		void onConnected(QString&);
		void onDisconnected(QString&);
		void onToConnect(QString&);

	private:
		QWidget* pOneCloudContainer;
		OneCloudMainWindow* pFrontOneCloudMainWindow;

		QMap<QString, OneCloudMainWindow*> oneCloudMap;
		QList<OneCloudMainWindow*> oneCloudList;
	};
}

