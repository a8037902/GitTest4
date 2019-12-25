#include "application.h"
#include <QDebug>
#include <QWidget>
#include <QtNetwork/QLocalSocket>
#include <QtNetwork/QLocalServer>
#include <QFileInfo>
#include <QLibrary>
#include "settings.h"

Application::Application(int &argc, char **argv, int flags)
	:QApplication(argc,argv,flags)
	, window(0)
	, bRunning(false)
	, localServer(NULL)
{

	int time = SingleTon<Settings>::getInstance().oneValue("Globa","autoLockTime", 30).toInt();
	//新建一个QTimer对象  
	timer = new QTimer();
	//设置定时器每个多少毫秒发送一个timeout()信号  
	//timer->setInterval(time*60000);
	//启动定时器  
	timer->start(time * 60000);

	// 取应用程序名作为LocalServer的名字
	serverName = QFileInfo(QCoreApplication::applicationFilePath()).fileName();
	//qDebug()<<serverName;
	initLocalConnection();
}


Application::~Application()
{
}
bool Application::notify(QObject *obj, QEvent *e)
{
	//qDebug() << e->type();
	if (e->type() >= QEvent::MouseButtonPress && e->type() <= QEvent::KeyRelease)
	{
		if (window)
		{
			int time = SingleTon<Settings>::getInstance().oneValue("Globa", "autoLockTime", 30).toInt();
			timer->stop();
			timer->start(time*60000);
			//qDebug() << "stop";
		}
	}
	/*else
	{
		if (!timer->isActive())
		{
			timer->start();
			qDebug() << "start";
		}
	}*/

	return QApplication::notify(obj, e);
}

void Application::setWindowInstance(PowerdudeMain *wnd)
{
	window = wnd;
	//信号和槽  
	connect(timer, SIGNAL(timeout()), wnd, SLOT(onTimerOut()));
}




////////////////////////////////////////////////////////////////////////////////
// 说明：
// 检查是否已經有一个实例在运行, true - 有实例运行， false - 没有实例运行
////////////////////////////////////////////////////////////////////////////////
bool Application::isRunning()
{
	return bRunning;
}

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 通过socket通讯实现程序单实例运行，监听到新的连接时触发该函数
////////////////////////////////////////////////////////////////////////////////
void Application::newLocalConnection()
{
	QLocalSocket *socket = localServer->nextPendingConnection();
	if (!socket)
		return;
	socket->waitForReadyRead(1000);
	QTextStream stream(socket);
	//其他处理
	delete socket;
	if (window != NULL && !window->isHidden())
	{
		//激活窗口
		window->raise();
		window->activateWindow();
		window->setWindowState((window->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
		window->show();
	}
}

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 通过socket通讯实现程序单实例运行，
// 初始化本地连接，如果连接不上server，则创建，否则退出
////////////////////////////////////////////////////////////////////////////////
void Application::initLocalConnection()
{
	bRunning = false;
	QLocalSocket socket;
	socket.connectToServer(serverName);
	if (socket.waitForConnected(500))
	{
		bRunning = true;
		// 其他处理，如：将启动参数发送到服务端
		QTextStream stream(&socket);
		QStringList args = QCoreApplication::arguments();
		if (args.count() > 1)
			stream << args.last();
		else
			stream << QString();
		stream.flush();
		socket.waitForBytesWritten();

		return;
	}

	//连接不上服务器，就创建一个
	newLocalServer();
}

////////////////////////////////////////////////////////////////////////////////
// 说明：
// 创建LocalServer
////////////////////////////////////////////////////////////////////////////////
void Application::newLocalServer()
{
	localServer = new QLocalServer(this);
	connect(localServer, SIGNAL(newConnection()), this, SLOT(newLocalConnection()));
	if (!localServer->listen(serverName))
	{
		// 此时监听失败，可能是程序崩溃时,残留进程服务导致的,移除之
		if (localServer->serverError() == QAbstractSocket::AddressInUseError)
		{
			QLocalServer::removeServer(serverName); // <-- 重点
			localServer->listen(serverName); // 再次监听
		}
	}
}
