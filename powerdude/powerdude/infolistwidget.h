#pragma once
#include <QFrame.h>
#include "ui_infolistwidget.h"
#include "infowidget.h"


class InfoListWidget :
	public QFrame
{
	Q_OBJECT

	Q_PROPERTY(QString listName READ listName WRITE setListName)
	Q_PROPERTY(bool hasBtn READ isHasBtn WRITE setHasBtn)
public:
	explicit InfoListWidget(QWidget *parent = Q_NULLPTR);
	~InfoListWidget();

	void resizeFrame();

	QString listName() const;
	void setListName(QString);

	bool isHasBtn() const;
	void setHasBtn(bool);

	InfoWidget* addInfoWidget(QString & key, QString &name, int type, InfoWidget::ConnectStatus status);
	InfoWidget* addInfoWidget(QString & key, QString &url, QString &name, int type, bool isRelevance=true, InfoWidget::ConnectStatus status= InfoWidget::Disconnecting);
	InfoWidget* getInfoWidget(QString & key);
	void removeInfoWidget(QString & key);
	void clearInfoWidget();
	void clearNotOpenInfoWidget();

	bool isXBtnShow();

	void hideX(QString & key);
	void showX(QString & key);

	void hideUpdateBtn()
	{
		ui->updateBtn->hide();
	}
	void showUpdateBtn()
	{
		ui->updateBtn->show();
	}

signals:
	void openWidget(QString&);
	void connectWidget(QString&);
	void disconnectWidget(QString&);
	void closeWidget(QString&);
	void updateWidget();

public slots:
	void slot_ScroolWidget(int value);

	void onOpenWidget();
	void onConnectWidget();
	void onDisconnectWidget();
	void onCloseWidget();
	void on_updateBtn_clicked();

	void onOneCloudOpened(QString &);
	void onOneCloudConnected(QString &);
	void onOneCloudDisconnected(QString &);
	void onOneCloudCloseed(QString &);
	void onOneCloudToConnect(QString &);

protected:
	void disconnectInfoWidget(InfoWidget* pInfoWidget);

private:
	Ui::InfoListWidget* ui;

	QMap<QString, InfoWidget*> infoWidgetMap;
};

