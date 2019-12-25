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
	//�½�һ��QTimer����  
	timer = new QTimer();
	//���ö�ʱ��ÿ�����ٺ��뷢��һ��timeout()�ź�  
	//timer->setInterval(time*60000);
	//������ʱ��  
	timer->start(time * 60000);

	// ȡӦ�ó�������ΪLocalServer������
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
	//�źźͲ�  
	connect(timer, SIGNAL(timeout()), wnd, SLOT(onTimerOut()));
}




////////////////////////////////////////////////////////////////////////////////
// ˵����
// ����Ƿ��ѽ���һ��ʵ��������, true - ��ʵ�����У� false - û��ʵ������
////////////////////////////////////////////////////////////////////////////////
bool Application::isRunning()
{
	return bRunning;
}

////////////////////////////////////////////////////////////////////////////////
// ˵����
// ͨ��socketͨѶʵ�ֳ���ʵ�����У��������µ�����ʱ�����ú���
////////////////////////////////////////////////////////////////////////////////
void Application::newLocalConnection()
{
	QLocalSocket *socket = localServer->nextPendingConnection();
	if (!socket)
		return;
	socket->waitForReadyRead(1000);
	QTextStream stream(socket);
	//��������
	delete socket;
	if (window != NULL && !window->isHidden())
	{
		//�����
		window->raise();
		window->activateWindow();
		window->setWindowState((window->windowState() & ~Qt::WindowMinimized) | Qt::WindowActive);
		window->show();
	}
}

////////////////////////////////////////////////////////////////////////////////
// ˵����
// ͨ��socketͨѶʵ�ֳ���ʵ�����У�
// ��ʼ���������ӣ�������Ӳ���server���򴴽��������˳�
////////////////////////////////////////////////////////////////////////////////
void Application::initLocalConnection()
{
	bRunning = false;
	QLocalSocket socket;
	socket.connectToServer(serverName);
	if (socket.waitForConnected(500))
	{
		bRunning = true;
		// ���������磺�������������͵������
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

	//���Ӳ��Ϸ��������ʹ���һ��
	newLocalServer();
}

////////////////////////////////////////////////////////////////////////////////
// ˵����
// ����LocalServer
////////////////////////////////////////////////////////////////////////////////
void Application::newLocalServer()
{
	localServer = new QLocalServer(this);
	connect(localServer, SIGNAL(newConnection()), this, SLOT(newLocalConnection()));
	if (!localServer->listen(serverName))
	{
		// ��ʱ����ʧ�ܣ������ǳ������ʱ,�������̷����µ�,�Ƴ�֮
		if (localServer->serverError() == QAbstractSocket::AddressInUseError)
		{
			QLocalServer::removeServer(serverName); // <-- �ص�
			localServer->listen(serverName); // �ٴμ���
		}
	}
}
