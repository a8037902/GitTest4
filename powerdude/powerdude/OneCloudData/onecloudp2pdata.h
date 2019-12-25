#pragma once
#include <QObject>
#include <QMap>
#include <QVariant>
#include "OneCloud/oneclouddata.h"
namespace OneCloud {
	class OneCloudP2PData :
		public OneCloudData
	{
	public:
		OneCloudP2PData();
		OneCloudP2PData(int type);
		~OneCloudP2PData();

		virtual QVariant getData(const QString &key);
		virtual void getDataAsyn(QString &key);
		virtual void setData(const QString& key, QVariant&data);

		virtual QString createKey();
	};
}
