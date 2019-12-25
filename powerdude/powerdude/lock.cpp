#include "Lock.h"
#include <QTimer>
#include <QDateTime>
#include <QJsonDocument>
#include <QJsonObject>
#include <qDebug>
#include "httpclient.h"
#include "onecloudmessagebox.h"

Lock::Lock(QWidget *parent)
	:QWidget(parent)
	, ui(new Ui::Lock())
{
	ui->setupUi(this);

	Qt::WindowFlags flags =
		(windowFlags());
	setWindowFlags(Qt::WindowType::FramelessWindowHint);

	showFullScreen();

	ui->horizontalFrame->setGeometry(QRect(this->size().width() / 2 - 100, this->size().height() / 2 - 20, 200, 40));
	ui->horizontalFrame->show();

	QPalette palette;
	QPixmap bgImage(":/powerdudemain/Resources/u443.png");
	bgImage.scaled(this->size());//, Qt::IgnoreAspectRatio, Qt::SmoothTransformation
	palette.setBrush(QPalette::Background, QBrush(bgImage.scaled(this->size())));
	this->setPalette(palette);

	ui->textMovingWidget->addText(QStringLiteral("天空不曾留下飞鸟的痕迹，但我已飞过。"));
	ui->textMovingWidget->addText(QStringLiteral("书犹药也，善读可以医愚。"));
	ui->textMovingWidget->setGeometry(this->size().width() / 2 - 150, this->size().height() / 2 + 300, 300, 40);

	ui->headLabel->setGeometry(this->size().width() / 2-50, this->size().height() / 2 - 200, 100, 100);
	ui->verticalLayoutWidget->setGeometry(this->size().width() / 2-60, this->size().height() / 2 - 100, 120, 60);

	ui->verticalFrame_2->setGeometry(this->size().width() - 150, this->size().height() - 150, 100, 50);
	ui->verticalFrame_2->hide();

	ui->sysBtn->setGeometry(this->size().width()  - 100, this->size().height() - 100, 50, 50);

	ui->dateLabel->setGeometry(40, 40, 360, 40);

	QHBoxLayout *layout = new QHBoxLayout();
	layout->setContentsMargins(0, 0, 0, 0);//设置外边距
	layout->addStretch();
	layout->addWidget(ui->checkBox, 0, Qt::AlignRight);
	layout->addWidget(ui->loginBtn, 0, Qt::AlignRight);


	connect(ui->checkBox, &QCheckBox::stateChanged, [&](int iToggled) {
		if (iToggled) {
			ui->lineEdit->setEchoMode(QLineEdit::EchoMode::Password);
		}
		else {
			ui->lineEdit->setEchoMode(QLineEdit::EchoMode::Normal);
		}
	});

	ui->lineEdit->setTextMargins(0, 0, ui->checkBox->width() + ui->loginBtn->width() + 10, 0);
	ui->lineEdit->setLayout(layout);


	QTimer *timer = new QTimer(this);
	connect(timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
	timer->start(1000);
	timerUpdate();
}


Lock::~Lock()
{

}

void Lock::setUserInfo(UserInfo &userInfo)
{
	if (!userInfo.name.isEmpty())ui->nameLabel->setText(userInfo.name);
	if (!userInfo.mobile.isEmpty())ui->numLabel->setText(userInfo.mobile);
	if (userInfo.pixmap!=NULL)ui->headLabel->setPixmap(*userInfo.pixmap);
}

void Lock::setChatServerApi(QString & api, QString &token)
{
	chatServerApi = api;
	this->token = token;
}


void Lock::on_loginBtn_clicked()
{
	//验证密码
	HttpClient(chatServerApi + "/business/user/checkpassword")
		.addParam("token", token)
		.addParam("password",ui->lineEdit->text())
		.addHeader("Oc_Platform_AppVersion", "3.0.0")
		.addHeader("Oc_Chatserver_MinVersion", "3.0.0")
		.get([=](const QString &response) mutable {
		qDebug() << response;

		QJsonParseError jsonerror;
		QJsonDocument document = QJsonDocument::fromJson(response.toUtf8(), &jsonerror);

		if (!document.isNull() && jsonerror.error == QJsonParseError::NoError) {
			QJsonObject jsonObject = document.object();
			int code = document.object().value("code").toInt();
			if (0 == code) {
				emit unLock();
				/*QJsonArray jsonObjectData = jsonObject.value("data").toArray();
				for (int i = 0; i < jsonObjectData.count(); i++) {

				}*/
			}
			else {
				OneCloudMessageBox::information(this, QStringLiteral("密码认证失败，请重新输入！"));
			}
		}
	});
	//emit unLock();
	//this->close();
}
void Lock::on_minBtn_clicked()
{
	showMinimized();
	//emit minClicked();
}
void Lock::on_exitBtn_clicked()
{
	emit exitClicked();
	//this->close();
}
void Lock::on_sysBtn_clicked()
{
	if (ui->verticalFrame_2->isHidden()) {
		ui->verticalFrame_2->show();
	}
	else {
		ui->verticalFrame_2->hide();
	}
}
void Lock::timerUpdate()
{
	QDateTime time = QDateTime::currentDateTime();

	QString str = time.toString("yyyy-MM-dd  dddd  hh:mm:ss");

	ui->dateLabel->setText(str);
}

void Lock::keyReleaseEvent(QKeyEvent * event)
{
	switch (event->key()) {
	case Qt::Key_Return://数字键盘的Enter键
	case Qt::Key_Enter://Enter键
		on_loginBtn_clicked();
		break;
	default:
		break;
	}
	QWidget::keyReleaseEvent(event);
}
