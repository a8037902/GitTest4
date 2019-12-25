#include "onecloudview.h"
#include <qDebug>
#include "translucentbutton.h"

namespace OneCloud {
	OneCloudView::OneCloudView(OneCloudData *pData, QWidget *parent)
		: QObject()
	{
		oneCloudData = pData;
	}

	OneCloudView::OneCloudView(QWidget *parent)
		: QObject()
	{

	}


	OneCloudView::~OneCloudView()
	{

	}
	void OneCloudView::setOneCloudData(const QString & key, QVariant & data)
	{
		oneCloudData->setData(key, data);
		//oneCloudDataMap.insert(key, data);
	}

	QVariant OneCloudView::getOneCloudData(const QString & key)
	{
		return oneCloudData->getData(key);
	}
}
