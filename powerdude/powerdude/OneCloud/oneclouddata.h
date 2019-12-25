#pragma once
#include <QObject>
#include <QMap>
#include <QVariant>
namespace OneCloud {
	class OneCloudData :
		public QObject
	{
		Q_OBJECT
	public:
		OneCloudData();
		OneCloudData(int type);
		~OneCloudData();

		int getType();

		virtual QVariant getData(const QString &key);
		virtual void getDataAsyn(QString &key);
		virtual void setData(const QString& key, QVariant&data);
		virtual void setData(const QString& key, int data);
		virtual void setData(const QString& key, QString& data);
		virtual void setData(QMap<QString, QVariant>&map);

		virtual QString createKey();

	signals:
		void dataAsyn(QVariant&);
		void dataAsyn(QList<QVariant>&);

	protected:
		QMap<QString, QVariant> oneCloudDataMap;

	private:
		int type;
	};
}
