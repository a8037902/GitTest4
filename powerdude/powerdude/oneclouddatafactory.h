#pragma once
#include <QJsonObject>
#include "OneCloud/OneCloudData.h"
#include "OneCloud/OneCloudView.h"
using namespace OneCloud;
class OneCloudDataFactory:
	public QObject
{
public:
	OneCloudDataFactory();
	~OneCloudDataFactory();

	static OneCloudData* getOneCloudDataFromJsonObject(int type, QJsonObject &jsonObject);
	static OneCloudView* createOneCloudView(int type, OneCloudData *pData);
};

