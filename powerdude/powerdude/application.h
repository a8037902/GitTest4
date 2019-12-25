#pragma once
#include <qapplication.h>
#include "powerdudemain.h"


class QLocalServer;

class Application :
	public QApplication
{
	Q_OBJECT
public:
	Application(int &argc, char **argv, int = ApplicationFlags);
	virtual ~Application();

	bool notify(QObject *, QEvent *);
	void setWindowInstance(PowerdudeMain *wnd);

	bool isRunning();               // 是否已经有实例在运行

private slots:
	// 有新连接时触发
	void newLocalConnection();

private:
	// 初始化本地连接
	void initLocalConnection();
	// 创建服务端
	void newLocalServer();
	bool bRunning;                  // 是否已经有实例在运行
	QLocalServer *localServer;      // 本地socket Server
	QString serverName;             // 服务名称

private:
	PowerdudeMain* window; //保存一个你窗体的指针  
	QTimer *timer;
};

