#pragma once
#include <QLabel>
#include <QWebSocket>
#include <QTimer>
#include <QUuid>
#include <QMovie>
#include <QEvent.h>
#include "MstscRdpInfo.h"
class WebLabel :
	public QLabel
{
	Q_OBJECT
public:
	explicit WebLabel(QWidget* parent = nullptr);
	~WebLabel();

	void stopMovie();

	static QPixmap createQRCode(const QString &text);

signals:
	void textReceived(MstscRdpInfo &info);
	void clicked();
	void leave();
	void enter();
	void mouseMove(QCursor&);

public slots:
	/*void onDisconnected();
	void onConnected();
	void onTextReceived(QString msg);
	void onError(QAbstractSocket::SocketError error);
	void onSslErrors(const QList<QSslError> &errors);
	void reconnect();*/
	void onShowUrl(QString url);
	void onShowMovie();
	void onDisconnected();

	//void onLoginComplete();
protected:
	void showEvent(QShowEvent *event);
	void hideEvent(QHideEvent *event);
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void mouseReleaseEvent(QMouseEvent *ev);
	void leaveEvent(QEvent * event)override;
	void enterEvent(QEvent * event)override;
	void mouseMoveEvent(QMouseEvent *event)override;

private:
	//QWebSocket * _pdataRecvWS;
	//QTimer * _ptimer;
	//QString _uuid;
	QMovie *movie;
	QLabel * pLogo;
	QString url;
};

