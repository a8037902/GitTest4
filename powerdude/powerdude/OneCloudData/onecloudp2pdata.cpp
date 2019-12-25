#include "onecloudp2pdata.h"


namespace OneCloud {
	OneCloudP2PData::OneCloudP2PData()
	{
	}

	OneCloudP2PData::OneCloudP2PData(int type)
		:OneCloudData(type)
	{
	}


	OneCloudP2PData::~OneCloudP2PData()
	{
	}
	QVariant OneCloudP2PData::getData(const QString & key)
	{
		return OneCloudData::getData(key);
	}
	void OneCloudP2PData::getDataAsyn(QString & key)
	{
		OneCloudData::getDataAsyn(key);
	}
	void OneCloudP2PData::setData(const QString & key, QVariant & data)
	{
		OneCloudData::setData(key, data);
	}
	QString OneCloudP2PData::createKey()
	{
		return QString();
	}
}
