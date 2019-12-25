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

	//// 创建信号量
	//QSystemSemaphore semaphore("SingleAppTest2Semaphore", 1);
	//// 启用信号量，禁止其他实例通过共享内存一起工作
	//semaphore.acquire();

#ifndef Q_OS_WIN32
	// 在linux / unix 程序异常结束共享内存不会回收
	// 在这里需要提供释放内存的接口，就是在程序运行的时候如果有这段内存 先清除掉
	/*QSharedMemory nix_fix_shared_memory("SingleAppTest2");
	if (nix_fix_shared_memory.attach())
	{
		nix_fix_shared_memory.detach();
	}*/
#endif
	//// 创建一个共享内存  “SingleAppTest2”表示一段内存的标识key 可作为唯一程序的标识
	//QSharedMemory sharedMemory("SingleAppTest2");
	//// 测试是否已经运行
	//bool isRunning = false;
	//// 试图将共享内存的副本附加到现有的段中。
	//if (sharedMemory.attach())
	//{
	//	// 如果成功，则确定已经存在运行实例
	//	isRunning = true;
	//}
	//else
	//{
	//	// 否则申请一字节内存
	//	sharedMemory.create(1);
	//	// 确定不存在运行实例
	//	isRunning = false;
	//}
	//semaphore.release();

	//// 如果您已经运行了应用程序的一个实例，那么我们将通知用户。
	//if (isRunning)
	//{
	//	OneCloudMessageBox::warning(NULL, QStringLiteral("警告"), QStringLiteral("小智已启动，请勿重复启动。"));
	//	return 1;
	//}
	if (!a.isRunning()) {
		QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::PluginsEnabled, true);
		QWebEngineSettings::defaultSettings()->setAttribute(QWebEngineSettings::ScreenCaptureEnabled, true);//启用屏幕捕捉屏幕共享
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
