#include "powerdude.h"
#include "application.h"
#include "powerdudewin.h"
#include "powerdudemain.h"
#include "hook.h"
#include "settings.h"
#include "setdialog.h"
#include <QtWidgets/QApplication>
#include <QSurfaceFormat>
#include <QWebEngineSettings>
#include "update.h"
#include "version.h"
#include <QFontDatabase>
#include <QMessageBox>
#include "OneCloudMessageBox.h"
#include "dlog.h"

//export QWS_DISPLAY = "LinuxFB:mmWidth95:0";
//export QWS_SIZE = "1440x900";


int main(int argc, char *argv[])
{

	/*QSurfaceFormat surface;
	surface.setAlphaBufferSize(8);
	QSurfaceFormat::setDefaultFormat(surface);*/

	QGuiApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
	QCoreApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);
	QApplication::setAttribute(Qt::AA_UseSoftwareOpenGL);

#ifdef QT_NO_DEBUG
	qInstallMessageHandler(myMsgOutput);
#endif

	Application a(argc, argv);

	//// �����ź���
	//QSystemSemaphore semaphore("SingleAppTest2Semaphore", 1);
	//// �����ź�������ֹ����ʵ��ͨ�������ڴ�һ����
	//semaphore.acquire();

#ifndef Q_OS_WIN32
	// ��linux / unix �����쳣���������ڴ治�����
	// ��������Ҫ�ṩ�ͷ��ڴ�Ľӿڣ������ڳ������е�ʱ�����������ڴ� �������
	/*QSharedMemory nix_fix_shared_memory("SingleAppTest2");
	if (nix_fix_shared_memory.attach())
	{
		nix_fix_shared_memory.detach();
	}*/
#endif
	//// ����һ�������ڴ�  ��SingleAppTest2����ʾһ���ڴ�ı�ʶkey ����ΪΨһ����ı�ʶ
	//QSharedMemory sharedMemory("SingleAppTest2");
	//// �����Ƿ��Ѿ�����
	//bool isRunning = false;
	//// ��ͼ�������ڴ�ĸ������ӵ����еĶ��С�
	//if (sharedMemory.attach())
	//{
	//	// ����ɹ�����ȷ���Ѿ���������ʵ��
	//	isRunning = true;
	//}
	//else
	//{
	//	// ��������һ�ֽ��ڴ�
	//	sharedMemory.create(1);
	//	// ȷ������������ʵ��
	//	isRunning = false;
	//}
	//semaphore.release();

	//// ������Ѿ�������Ӧ�ó����һ��ʵ������ô���ǽ�֪ͨ�û���
	//if (isRunning)
	//{
	//	OneCloudMessageBox::warning(NULL, QStringLiteral("����"), QStringLiteral("С���������������ظ�������"));
	//	return 1;
	//}
	if (!a.isRunning()) {
		QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
		QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);//������Ļ��׽��Ļ����
		QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::AllowRunningInsecureContent, true);
		QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::WebGLEnabled, true);
		QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::FullScreenSupportEnabled, true);

		QFontDatabase::addApplicationFont("SourceHanSansCN-Regular.otf");

		Version *ver = new Version();
		QString cur_version = ver->getVersion();
		a.setApplicationVersion(cur_version);

		//Powerdude w;
		PowerdudeMain w;
		a.setWindowInstance(&w);
		w.startHookRDPDisplay();

		SingleTon<Settings>::getInstance().beginGroup("Globa");
		bool en = SingleTon<Settings>::getInstance().value("hotKeyEn", true).toBool();
		bool autoUpdate = SingleTon<Settings>::getInstance().value("autoUpdate", false).toBool();
		QString jsonUrl = SingleTon<Settings>::getInstance().value("updateJson", DEFAULT_UPDATE_URL).toString();
		SingleTon<Settings>::getInstance().endGroup();
		SingleTon<Hook>::getInstance().disableRDPAskWindow();

		QObject::connect(&SingleTon<Hook>::getInstance(), SIGNAL(shortcut(const QString &)), &w, SLOT(onShortcut(const QString &)));

		SingleTon<Hook>::getInstance().hook(true);

		if (autoUpdate) {
			Update *pb_update = new Update();
			QObject::connect(pb_update, SIGNAL(getVersion(QString &)), &w, SLOT(onGetVersion(QString &)));
			QString  latest_ver = pb_update->query_version(jsonUrl);
		}
		w.show();
		int iResult = a.exec();
		SingleTon<Hook>::getInstance().disableRDPAskWindow(false);
		return iResult;
	}
	return 0;
}
