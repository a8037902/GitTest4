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

	bool isRunning();               // �Ƿ��Ѿ���ʵ��������

private slots:
	// ��������ʱ����
	void newLocalConnection();

private:
	// ��ʼ����������
	void initLocalConnection();
	// ���������
	void newLocalServer();
	bool bRunning;                  // �Ƿ��Ѿ���ʵ��������
	QLocalServer *localServer;      // ����socket Server
	QString serverName;             // ��������

private:
	PowerdudeMain* window; //����һ���㴰���ָ��  
	QTimer *timer;
};

