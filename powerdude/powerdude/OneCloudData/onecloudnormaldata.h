#pragma once
#include <QObject>
#include <QMap>
#include <QVariant>
#include "OneCloud/oneclouddata.h"
namespace OneCloud {
	class OneCloudNormalData :
		public OneCloudData
	{
	public:
		OneCloudNormalData();
		OneCloudNormalData(int type);
		~OneCloudNormalData();

		virtual QVariant getData(const QString &key);
		virtual void getDataAsyn(QString &key);
		virtual void setData(const QString& key, QVariant&data);

		virtual QString createKey();
	};
}
