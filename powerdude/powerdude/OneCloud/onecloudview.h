#pragma once
#include <qwidget.h>
#include "oneclouddata.h"

namespace OneCloud {
	class OneCloudView :
		public QObject
	{
		Q_OBJECT
	public:
		OneCloudView(OneCloudData *pData, QWidget* parent = nullptr);
		OneCloudView(QWidget* parent = nullptr);
		~OneCloudView();

		virtual void init(QWidget* pOneCloudContainer) = 0;
		virtual void init(QWidget* pOneCloudContainer, QRect &rect) = 0;

		virtual void setHidden(bool hidden)=0;
		virtual void show()=0;
		virtual void hide()=0;

		virtual void disconnectHost()=0;
		virtual void connectHost()=0;

		void setOneCloudData(const QString& key, QVariant&data);
		QVariant getOneCloudData(const QString&key);
	signals:
		void connected();
		void disconnected();
	private:
		//QMap<QString, QVariant> oneCloudDataMap;
		OneCloudData *oneCloudData;
	};
}

