#include "infolistwidget.h"
#include <qDebug>
#include "font.h"

InfoListWidget::InfoListWidget(QWidget* parent)
	:QFrame(parent)
	, ui(new Ui::InfoListWidget())
{
	ui->setupUi(this);

	ui->verticalScrollBar->hide();
	connect(ui->verticalScrollBar, SIGNAL(valueChanged(int)), this, SLOT(slot_ScroolWidget(int)));

	Font font;
	font.setPixelSize(20);
	font.setBold(true);
	ui->label->setFont(font);
}


InfoListWidget::~InfoListWidget()
{
}

void InfoListWidget::slot_ScroolWidget(int value)
{
	double p = static_cast<double>(value) / static_cast<double>(ui->verticalScrollBar->maximum());
	ui->bodyFrame->move(0, -(ui->bodyFrame->height() - ui->verticalScrollBar->height())*p);
}

void InfoListWidget::resizeFrame()
{

	int num = infoWidgetMap.size();
	int h = 90 * num;// +6 * (num + 1);
	ui->bodyFrame->setMinimumHeight(h);
	if (this->height() < h + 78) {
		ui->verticalScrollBar->show();
		ui->verticalScrollBar->setValue(0);
	}
	else {
		ui->verticalScrollBar->hide();
	}
}

QString InfoListWidget::listName() const
{
	return ui->label->text();
}

void InfoListWidget::setListName(QString text)
{
	ui->label->setText(text);
}

bool InfoListWidget::isHasBtn() const
{
	return false;
}

void InfoListWidget::setHasBtn(bool has)
{
	if (has) {
		ui->openLoginBtn->show();
	}
	else {
		ui->openLoginBtn->hide();
	}
}

InfoWidget* InfoListWidget::addInfoWidget(QString & key, QString &name,int type, InfoWidget::ConnectStatus status)
{
	if (name.isEmpty()) {
		name = QStringLiteral("口令桌面");
	}
	if (!ui->infoLabel->isHidden()) {
		ui->infoLabel->hide();
	}
	if (!infoWidgetMap.contains(key)) {
		InfoWidget* pInfoWidget = new InfoWidget(ui->bodyFrame);
		ui->verticalLayout_3->addWidget(pInfoWidget);
		pInfoWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		pInfoWidget->setMinimumHeight(90);
		pInfoWidget->setName(name);
        if (type == 100) {
            QString text = QString::fromLocal8Bit("包含%1个桌面").arg(key.section('@', 2, 2));
            pInfoWidget->setText(text);
        }
        else if (type == 40 || type == 41) {
            pInfoWidget->setText(key.section('@', 0, 0));
        }
        else {
            pInfoWidget->setText(key);
        }
		pInfoWidget->setKey(key);
		pInfoWidget->setType(type);
		if (type != 0) {
			pInfoWidget->setRelevance(true);
		}
		pInfoWidget->setConnectStatus(status);
		//pInfoWidget->setGeometry(0, 90 * remoteDesktopNum, ui->remoteDesktopFrame->width(), 90);
		//remoteDesktopNum++;
		infoWidgetMap.insert(key, pInfoWidget);

		resizeFrame();


		connect(pInfoWidget, SIGNAL(clicked()), this, SLOT(onOpenWidget()));
		connect(pInfoWidget, SIGNAL(toConnect()), this, SLOT(onConnectWidget()));
		connect(pInfoWidget, SIGNAL(toDisconnect()), this, SLOT(onDisconnectWidget()));
		connect(pInfoWidget, SIGNAL(toClose()), this, SLOT(onCloseWidget()));
		return pInfoWidget;
	}
	return infoWidgetMap[key];
}

InfoWidget * InfoListWidget::addInfoWidget(QString & key, QString & url, QString & name, int type, bool isRelevance, InfoWidget::ConnectStatus status)
{
	if (name.isEmpty()) {
		name = QStringLiteral("口令桌面");
	}
	if (!ui->infoLabel->isHidden()) {
		ui->infoLabel->hide();
	}
	if (!infoWidgetMap.contains(key)) {
		InfoWidget* pInfoWidget = new InfoWidget(ui->bodyFrame);
		ui->verticalLayout_3->addWidget(pInfoWidget);
		pInfoWidget->setSizePolicy(QSizePolicy::Preferred, QSizePolicy::Fixed);
		pInfoWidget->setMinimumHeight(90);
		pInfoWidget->setName(name);
		pInfoWidget->setText(url);
		pInfoWidget->setKey(key);
		pInfoWidget->setType(type);
		pInfoWidget->setRelevance(isRelevance);
		pInfoWidget->setConnectStatus(status);
		pInfoWidget->hideX();
		//pInfoWidget->setGeometry(0, 90 * remoteDesktopNum, ui->remoteDesktopFrame->width(), 90);
		//remoteDesktopNum++;
		infoWidgetMap.insert(key, pInfoWidget);

		resizeFrame();


		connect(pInfoWidget, SIGNAL(clicked()), this, SLOT(onOpenWidget()));
		connect(pInfoWidget, SIGNAL(toConnect()), this, SLOT(onConnectWidget()));
		connect(pInfoWidget, SIGNAL(toDisconnect()), this, SLOT(onDisconnectWidget()));
		connect(pInfoWidget, SIGNAL(toClose()), this, SLOT(onCloseWidget()));
		return pInfoWidget;
	}
	return infoWidgetMap[key];
}

InfoWidget * InfoListWidget::getInfoWidget(QString & key)
{
	if (infoWidgetMap.contains(key)) {
		InfoWidget *pInfoWidget = infoWidgetMap[key];
		return pInfoWidget;
	}
	return nullptr;
}

void InfoListWidget::clearInfoWidget()
{
	QMap<QString, InfoWidget*>::const_iterator i;
	for (i = infoWidgetMap.constBegin(); i != infoWidgetMap.constEnd(); ++i) {
		InfoWidget *pInfoWidget = i.value();

		disconnectInfoWidget(pInfoWidget);
		pInfoWidget->deleteLater();

		emit closeWidget(pInfoWidget->getKey());
	}
	infoWidgetMap.clear();

	ui->infoLabel->show();
}

void InfoListWidget::clearNotOpenInfoWidget()
{
	QMap<QString, InfoWidget*>::iterator i;
	for (i = infoWidgetMap.begin(); i != infoWidgetMap.end();) {
		InfoWidget *pInfoWidget = i.value();

		if (!pInfoWidget->isXBtnShow())
		{
			disconnectInfoWidget(pInfoWidget);
			pInfoWidget->deleteLater();

			infoWidgetMap.erase(i++);  //#1
			emit closeWidget(pInfoWidget->getKey());
		}
		else
		{
			++i;
		}
	}
	//infoWidgetMap.clear();

	if (infoWidgetMap.size() == 0) {
		ui->infoLabel->show();
	}
}

bool InfoListWidget::isXBtnShow()
{
	QMap<QString, InfoWidget*>::const_iterator i;
	for (i = infoWidgetMap.constBegin(); i != infoWidgetMap.constEnd(); ++i) {
		InfoWidget *pInfoWidget = i.value();
		if (pInfoWidget->isXBtnShow()) {
			return true;
		}
	}
	return false;
}

void InfoListWidget::hideX(QString & key)
{
}

void InfoListWidget::showX(QString & key)
{
}

void InfoListWidget::onOpenWidget()
{
#ifdef _DEBUG
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
#endif
	InfoWidget *pInfoWidget = qobject_cast<InfoWidget *>(sender());
	emit openWidget(pInfoWidget->getKey());
}

void InfoListWidget::onConnectWidget()
{
#ifdef _DEBUG
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
#endif
	InfoWidget *pInfoWidget = qobject_cast<InfoWidget *>(sender());
	emit connectWidget(pInfoWidget->getKey());
}

void InfoListWidget::onDisconnectWidget()
{
	InfoWidget *pInfoWidget = qobject_cast<InfoWidget *>(sender());
	emit disconnectWidget(pInfoWidget->getKey());
}

void InfoListWidget::onCloseWidget()
{
#ifdef _DEBUG
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
#endif
	InfoWidget *pInfoWidget = qobject_cast<InfoWidget *>(sender());
	if (!pInfoWidget->getRelevance()) {
		disconnectInfoWidget(pInfoWidget);
		infoWidgetMap.remove(pInfoWidget->getKey());
		pInfoWidget->deleteLater();
		resizeFrame();

		if (infoWidgetMap.size() == 0) {
			ui->infoLabel->show();
		}
	}
	else {
		pInfoWidget->setConnectStatus(InfoWidget::Disconnecting);
		pInfoWidget->hideX();
	}

	emit closeWidget(pInfoWidget->getKey());
}

void InfoListWidget::on_updateBtn_clicked()
{
	emit updateWidget();
}

void InfoListWidget::onOneCloudOpened(QString &key)
{
	if (infoWidgetMap.contains(key)) {
		InfoWidget *pInfoWidget = infoWidgetMap[key];
		pInfoWidget->showX();
	}
}

void InfoListWidget::onOneCloudConnected(QString &key)
{
	if (infoWidgetMap.contains(key)) {
		InfoWidget *pInfoWidget = infoWidgetMap[key];
		pInfoWidget->setConnectStatus(InfoWidget::Connecting);
	}
}

void InfoListWidget::onOneCloudDisconnected(QString &key)
{
	if (infoWidgetMap.contains(key)) {
		InfoWidget *pInfoWidget = infoWidgetMap[key];
		pInfoWidget->setConnectStatus(InfoWidget::Disconnecting);
	}
}

void InfoListWidget::onOneCloudCloseed(QString &key)
{
	if (infoWidgetMap.contains(key)) {
		InfoWidget *pInfoWidget = infoWidgetMap[key];
		pInfoWidget->hideX();
	}
}

void InfoListWidget::onOneCloudToConnect(QString &key)
{
	if (infoWidgetMap.contains(key)) {
		InfoWidget *pInfoWidget = infoWidgetMap[key];
		pInfoWidget->setConnectStatus(InfoWidget::ToConnecting);
	}
}

void InfoListWidget::disconnectInfoWidget(InfoWidget * pInfoWidget)
{
	disconnect(pInfoWidget, SIGNAL(clicked()), this, SLOT(onOpenWidget()));
	disconnect(pInfoWidget, SIGNAL(toConnect()), this, SLOT(onConnectWidget()));
	disconnect(pInfoWidget, SIGNAL(toDisconnect()), this, SLOT(onDisconnectWidget()));
	disconnect(pInfoWidget, SIGNAL(toClose()), this, SLOT(onCloseWidget()));
}

void InfoListWidget::removeInfoWidget(QString & key)
{
	QMap<QString, InfoWidget*>::const_iterator i = infoWidgetMap.find(key);
	if (i != infoWidgetMap.end()) {
		InfoWidget *pInfoWidget = i.value();
		disconnectInfoWidget(pInfoWidget);
		pInfoWidget->deleteLater();

		emit closeWidget(key);
		infoWidgetMap.remove(key);
	}
	if (infoWidgetMap.size() == 0) {
		ui->infoLabel->show();
	}
}