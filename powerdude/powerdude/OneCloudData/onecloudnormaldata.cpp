#include "onecloudnormaldata.h"


namespace OneCloud {
	OneCloudNormalData::OneCloudNormalData()
	{
	}

	OneCloudNormalData::OneCloudNormalData(int type)
		:OneCloudData(type)
	{
	}


	OneCloudNormalData::~OneCloudNormalData()
	{
	}
	QVariant OneCloudNormalData::getData(const QString & key)
	{
		if (key == "url") {
			QString url = QString("%1@%2:%3").arg(OneCloudData::getData(QString("username")).toString()).arg(OneCloudData::getData(QString("ip")).toString()).arg(OneCloudData::getData(QString("port")).toString());
			return QVariant(url);
		}
		return OneCloudData::getData(key);
	}
	void OneCloudNormalData::getDataAsyn(QString & key)
	{
		OneCloudData::getDataAsyn(key);
	}
	void OneCloudNormalData::setData(const QString & key, QVariant & data)
	{
		OneCloudData::setData(key, data);
	}
	QString OneCloudNormalData::createKey()
	{
		QString key = QString("%1%2%3@%4:%5").arg(getType()).arg(oneCloudDataMap["type"].toString()).arg(oneCloudDataMap["username"].toString()).arg(oneCloudDataMap["ip"].toString()).arg(oneCloudDataMap["port"].toString());
		return key;
	}
}
