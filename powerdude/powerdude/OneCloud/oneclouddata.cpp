#include "oneclouddata.h"


namespace OneCloud {
	OneCloudData::OneCloudData()
	{
	}

	OneCloudData::OneCloudData(int type)
	{
		this->type = type;
	}

	OneCloudData::~OneCloudData()
	{
	}
	int OneCloudData::getType()
	{
		return type;
	}
	QVariant OneCloudData::getData(const QString & key)
	{
		if (oneCloudDataMap.contains(key)) {
			return oneCloudDataMap[key];
		}
		return QVariant();
	}
	void OneCloudData::getDataAsyn(QString & key)
	{
		if (oneCloudDataMap.contains(key)) {
			emit dataAsyn(oneCloudDataMap[key]);
		}
	}
	void OneCloudData::setData(const QString & key, QVariant & data)
	{
		oneCloudDataMap.insert(key, data);
	}
	void OneCloudData::setData(const QString & key, int data)
	{
		oneCloudDataMap.insert(key, data);
	}
	void OneCloudData::setData(const QString & key, QString & data)
	{
		oneCloudDataMap.insert(key, data);
	}
	void OneCloudData::setData(QMap<QString, QVariant>& map)
	{
		oneCloudDataMap = map;
	}
	QString OneCloudData::createKey()
	{
		return oneCloudDataMap["key"].toString();
	}
}
