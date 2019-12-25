#include "setdialog.h"
//#include <QSimpleUpdater.h>
#include "version.h"
#include <QNetworkReply>
#include <QNetworkAccessManager>
#include <QJsonDocument>
#include <QJsonValue>
#include <QJsonObject>
#include <QApplication>
#include <QDesktopWidget>
#include <QFileDialog>
#include <QDesktopServices>
#include <QMessageBox>
#include "settings.h"
#include "font.h"
#include "onecloudmessagebox.h"

const QString p2pModeText[3] = {
	QStringLiteral("自动根据网络情况建立网络连接"),
	QStringLiteral("建立隧道传输数据，速度更快"),
	QStringLiteral("使用中继服务器转发，适应场景广泛"),
};


//static const QString DEFS_URL = "https://raw.githubusercontent.com/alex-spataru/QSimpleUpdater/master/tutorial/definitions/updates.json";

SetDialog::SetDialog(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::SetDialog())
{
	ui->setupUi(this);

	setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);

	/*QObjectList list = children();
	for (int i = 0;i < list.size();i++) {
		if (list[i]->isWidgetType()) {
			((QWidget*)list[i])->setFont(Font());
		}
	}*/
	QList<QWidget*> list = findChildren<QWidget*>();
	for (int i = 0; i < list.size(); i++)
	{
		QWidget* widget = list.at(i);
		QFont font = widget->font();
		widget->setFont(Font());
	}

	/*int nWidth = QApplication::desktop()->screenGeometry().width()/2.4;
	this->setGeometry(QApplication::desktop()->screenGeometry().width() / 2 - nWidth / 2,
		QApplication::desktop()->screenGeometry().height() / 2 - nWidth / 4, 
		nWidth, nWidth/2);*/
	
	jsonUrl = SingleTon<Settings>::getInstance().oneValue("Globa", "updateJson", DEFAULT_UPDATE_URL).toString();

	m_updater = QSimpleUpdater::getInstance();

	pb_update = new Update();
	
	QObject::connect(pb_update, &Update::getVersion, this, &SetDialog::onGetVersion);
	QString  latest_ver = pb_update->query_version(jsonUrl);

	QString cur_version = qApp->applicationVersion();

	ui->upbtn3->setText(QString::fromLocal8Bit("当前版本：V%1").arg(cur_version));
	ui->updateBtn->setDisabled(true);
	

	m_pGroup = new QButtonGroup(this);
	m_pGroup->addButton(ui->exitLoginBtn,0);
	m_pGroup->addButton(ui->exitSysBtn,1);

	m_pLockGroup = new QButtonGroup(this);
	m_pLockGroup->addButton(ui->noLockBtn,0);
	m_pLockGroup->addButton(ui->autoLockBtn,1);

	SingleTon<Settings>::getInstance().beginGroup("Globa");
	ui->checkBox->setChecked(SingleTon<Settings>::getInstance().value("hotKeyEn", true).toBool());
	ui->checkBox_2->setChecked(SingleTon<Settings>::getInstance().value("altA", true).toBool());
	ui->checkBox_3->setChecked(SingleTon<Settings>::getInstance().value("altAHide", false).toBool());
	ui->checkBox_3->setEnabled(ui->checkBox_2->isChecked());
	ui->autoupdate_btn->setToggle(SingleTon<Settings>::getInstance().value("autoUpdate", false).toBool());
	ui->comboBox->setCurrentIndex(SingleTon<Settings>::getInstance().value("p2pMode").toInt());
	ui->p2pModeLabel->setText(p2pModeText[ui->comboBox->currentIndex()]);
	ui->timeSpinBox->setValue(SingleTon<Settings>::getInstance().value("autoLockTime", 30).toInt());
	ui->selfRunCBox->setChecked(SingleTon<Settings>::getInstance().value("autoStart", false).toBool());

	m_pGroup->button(SingleTon<Settings>::getInstance().value("exit", 0).toInt())->setChecked(true);
	m_pLockGroup->button(SingleTon<Settings>::getInstance().value("autoLock", 0).toInt())->setChecked(true);
	SingleTon<Settings>::getInstance().endGroup();

	QObject::connect(ui->autoupdate_btn, SIGNAL(toggled(bool)), this, SLOT(ontoggled(bool)));
	QObject::connect(ui->comboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurremtIndexChanged(int)));

	connect(m_pGroup, SIGNAL(buttonClicked(int)), this, SLOT(onExitButtonClicked(int)));
	connect(m_pLockGroup, SIGNAL(buttonClicked(int)), this, SLOT(onLockButtonClicked(int)));

	connect(ui->timeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(onValueChanged(int)));

	SingleTon<Settings>::getInstance().beginGroup("File");
	ui->filePathEdit->setText(SingleTon<Settings>::getInstance().value("filePath", QStandardPaths::standardLocations(QStandardPaths::DownloadLocation)).toString());
	ui->fileOpenCBox->setChecked(SingleTon<Settings>::getInstance().value("fileOpen",false).toBool());
	SingleTon<Settings>::getInstance().endGroup();
	

	connect(ui->filePathEdit, &QLineEdit::textChanged, [&](const QString&text) {
		SingleTon<Settings>::getInstance().setOneValue("File","filePath", text);
	});

	SingleTon<Settings>::getInstance().beginGroup("RemoteDesktop");
	ui->colorDepthCBox->setCurrentIndex(SingleTon<Settings>::getInstance().value("colorDepth",0).toInt());
	ui->desktopSizeCBox->setCurrentIndex(SingleTon<Settings>::getInstance().value("desktopSize",0).toInt());
	ui->fontCBox->setChecked(SingleTon<Settings>::getInstance().value("fontSmooth", 0).toBool());
	SingleTon<Settings>::getInstance().endGroup();
	connect(ui->colorDepthCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onColorDepthIndexChanged(int)));
	connect(ui->desktopSizeCBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onDesktopSizeIndexChanged(int)));

	SingleTon<Settings>::getInstance().beginGroup("RemoteSense");
	ui->remoteSenseNameEdit->setText(SingleTon<Settings>::getInstance().value("name").toString());
	ui->searchNearbyCBox->setChecked(SingleTon<Settings>::getInstance().value("searchNearby", false).toBool());
	SingleTon<Settings>::getInstance().endGroup();
	connect(ui->remoteSenseNameEdit, &QLineEdit::textChanged, [&](const QString&text) {
		SingleTon<Settings>::getInstance().setOneValue("RemoteSense", "name", text);
	});
}

SetDialog::~SetDialog()
{

}

void SetDialog::on_checkBox_clicked(bool isCheck)
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	//qDebug() << isCheck;
	SingleTon<Settings>::getInstance().setOneValue("Globa", "hotKeyEn", isCheck);
}

void SetDialog::on_checkBox_2_clicked(bool isCheck)
{
	SingleTon<Settings>::getInstance().setOneValue("Globa", "altA", isCheck);
	ui->checkBox_3->setEnabled(isCheck);
}

void SetDialog::on_checkBox_3_clicked(bool isCheck)
{
	SingleTon<Settings>::getInstance().setOneValue("Globa", "altAHide", isCheck);
}

void SetDialog::on_fileOpenCBox_clicked(bool isCheck)
{
	SingleTon<Settings>::getInstance().setOneValue("File", "fileOpen", isCheck);
}

void SetDialog::on_fontCBox_clicked(bool isCheck)
{
	SingleTon<Settings>::getInstance().setOneValue("RemoteDesktop", "fontSmooth", isCheck);
}

void SetDialog::on_selfRunCBox_clicked(bool isCheck)
{
	SingleTon<Settings>::getInstance().setOneValue("Globa", "autoStart", isCheck);
	QSettings  reg("HKEY_CURRENT_USER\\SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Run", QSettings::NativeFormat);

	if (isCheck)
	{
		QString strAppPath = QDir::toNativeSeparators(QCoreApplication::applicationFilePath());
		//strAppPath.replace(QChar('/'),QChar('\\'),Qt::CaseInsensitive);
		reg.setValue("wirtepad", strAppPath);
	}
	else
	{
		reg.setValue("wirtepad", "");
	}
}

void SetDialog::on_searchNearbyCBox_clicked(bool isCheck)
{
	SingleTon<Settings>::getInstance().setOneValue("RemoteSense", "searchNearby", isCheck);
}


void SetDialog::onCurremtIndexChanged(int index)
{
	SingleTon<Settings>::getInstance().setOneValue("Globa","p2pMode", index);
	ui->p2pModeLabel->setText(p2pModeText[index]);
}

void SetDialog::onColorDepthIndexChanged(int index)
{
	SingleTon<Settings>::getInstance().setOneValue("RemoteDesktop","colorDepth", index);
}

void SetDialog::onDesktopSizeIndexChanged(int index)
{
	SingleTon<Settings>::getInstance().setOneValue("RemoteDesktop", "desktopSize", index);
}

void SetDialog::onExitButtonClicked(int index)
{
	SingleTon<Settings>::getInstance().setOneValue("Globa","exit", index);
}

void SetDialog::onLockButtonClicked(int index)
{
	SingleTon<Settings>::getInstance().setOneValue("Globa", "autoLock", index);
}

void SetDialog::onValueChanged(int index)
{
	SingleTon<Settings>::getInstance().setOneValue("Globa", "autoLockTime", index);
}

void SetDialog::onGetVersion(QString & version)
{
	QString cur_version = qApp->applicationVersion();
	QString latest_ver = version;

	bool isvalid = pb_update->check_updatefile(pb_update->file_path);

	if (pb_update->compare(latest_ver, cur_version) && isvalid) {
		ui->upbtn1->setText(QString::fromLocal8Bit("请您及时更新到最新版本！"));
		ui->upbtn3->setText(QString::fromLocal8Bit("当前版本：V%1").arg(cur_version));
		ui->upbtn2->setText(QString::fromLocal8Bit("最新版本：V%1").arg(latest_ver));
		ui->updateBtn->setDisabled(false);
	} else {
		ui->upbtn1->setText(QString::fromLocal8Bit("当前是最新版本!"));
		ui->upbtn3->setText(QString::fromLocal8Bit("当前版本：V%1").arg(cur_version));
		if(!isvalid)
			ui->upbtn2->setText(QString::fromLocal8Bit("最新版本：文件无法访问"));
		else
			ui->upbtn2->setText(QString::fromLocal8Bit("最新版本：V%1").arg(latest_ver));
		ui->updateBtn->setDisabled(true);
	}
	QObject::disconnect(pb_update, &Update::getVersion, this, &SetDialog::onGetVersion);
}

void SetDialog::ontoggled(bool enable) {
	SingleTon<Settings>::getInstance().setOneValue("Globa","autoUpdate", enable);
}

void SetDialog::on_updateBtn_clicked()
{
	m_updater->checkForUpdates(jsonUrl);
}

void SetDialog::on_selectDirBtn_clicked()
{
	QString file_path = QFileDialog::getExistingDirectory(this, QStringLiteral("请选择保存路径..."), "./");
	if (file_path.isEmpty())
	{
		//ui->filePathEdit->
		return;
	}
	else
	{
		ui->filePathEdit->setText(file_path);
		/*SingleTon<Settings>::getInstance().beginGroup("Globa");
		SingleTon<Settings>::getInstance().setValue("filePath", file_path);
		SingleTon<Settings>::getInstance().endGroup();*/
		qDebug() << file_path << endl;
	}
}

void SetDialog::on_filePathBtn_clicked()
{
	SingleTon<Settings>::getInstance().beginGroup("File");
	QString path = SingleTon<Settings>::getInstance().value("filePath", QStandardPaths::standardLocations(QStandardPaths::DownloadLocation)).toString();
	SingleTon<Settings>::getInstance().endGroup();
	if (path.isEmpty()) {
		OneCloudMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("路径不存在，请重新输入。"));
		return;
	}
	bool ok = QDesktopServices::openUrl(QUrl::fromLocalFile(path));
	if (!ok) {
		mkMutiDir(path);
		bool ok = QDesktopServices::openUrl(QUrl::fromLocalFile(path));
		if (!ok) {
			OneCloudMessageBox::warning(this, QStringLiteral("警告"), QStringLiteral("无法创建路径，请重新输入。"));
		}
	}
}

QString SetDialog::mkMutiDir(const QString path) {
	QDir dir(path);
	if (dir.exists(path)) {
		return path;
	}
	QString parentDir = mkMutiDir(path.mid(0, path.lastIndexOf('/')));
	QString dirname = path.mid(path.lastIndexOf('/') + 1);
	QDir parentPath(parentDir);
	if (!dirname.isEmpty())
		parentPath.mkpath(dirname);
	return parentDir + "/" + dirname;
}

