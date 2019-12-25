#include "oneclouddatafactory.h"
#include "OneCloudData/onecloudp2pdata.h"
#include "OneCloudData/onecloudnormaldata.h"
#include "OneCloud/OneCloudAxWidget.h"
#include "OneCloud/OneCloudWebEngineView.h"
#include <QList>
#include "Settings.h"

const int ColorDepth[] = {
	32,24,16,15
};

const int DesktopSize[][2] = {
	{0,0},
	{1440,900},
	{1366,768},
	{1280,800},
};

OneCloudDataFactory::OneCloudDataFactory()
{
}


OneCloudDataFactory::~OneCloudDataFactory()
{
}

OneCloudData* OneCloudDataFactory::getOneCloudDataFromJsonObject(int type, QJsonObject &jsonObject) {
	OneCloudData* pOneCloudData = NULL;
	QList<QString> list;
	//QMap<QString, QVariant> map;
	switch (type) {
		case 2:
		{
			list.append("type");
			list.append("name");
			list.append("username");
			list.append("password");
			list.append("domain");
			int desktopType = jsonObject.value("type").toString().toInt();
			if (desktopType == 2) {
				list.append("p2pid");
				pOneCloudData = new OneCloudP2PData(type);
			}
			else {
				list.append("ip");
				list.append("port");
				pOneCloudData = new OneCloudNormalData(type);
			}
			SingleTon<Settings>::getInstance().beginGroup("RemoteDesktop");
			int colorDepth = ColorDepth[SingleTon<Settings>::getInstance().value("colorDepth", 0).toInt()];
			int desktopSize = SingleTon<Settings>::getInstance().value("desktopSize", 0).toInt();
			SingleTon<Settings>::getInstance().endGroup();
			int desktopWidth = DesktopSize[desktopSize][0];
			int desktopHeight = DesktopSize[desktopSize][1];
			pOneCloudData->setData("colorDepth", colorDepth);
			pOneCloudData->setData("desktopWidth", desktopWidth);
			pOneCloudData->setData("desktopHeight", desktopHeight);
			
			break;
		}
		case 4:
		{
			list.append("bizid");
			list.append("desktopname");
			list.append("username");
			list.append("password");
			list.append("domain");
			list.append("ip");
			list.append("port");
			list.append("subject");
			
			break;
		}
		case 14:
		case 101:
			list.append("url");
			list.append("key");
			list.append("name");
			list.append("downloadPath");
			pOneCloudData = new OneCloudData(type);
			break;
	}

	for (int i = 0;i < list.size();i++) {
		pOneCloudData->setData(list[i], jsonObject.value(list[i]).toVariant());
	}

	return pOneCloudData;
}

OneCloudView * OneCloudDataFactory::createOneCloudView(int type, OneCloudData * pData)
{
	OneCloudView * pView;
	if (type == 2) {
		pView = new OneCloudAxWidget(pData);
	}
	else {
		pView = new OneCloudWebEngineView(pData);
	}
	return pView;
}
