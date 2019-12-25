#pragma once

#include <QDialog>
#include <QSettings>
#include "websocket.h"
#include <qgraphicseffect.h>
#include <qpainter.h>

namespace Ui { class ScreenRecordDialog; };

class ScreenRecordDialog : public QDialog
{
	Q_OBJECT

public:
	ScreenRecordDialog(WebSocket* pWebSocket,int& iResult,QWidget *parent = Q_NULLPTR);
	~ScreenRecordDialog();

protected:
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);
	virtual void paintEvent(QPaintEvent *event);
private slots:
	void onScreenRecCloseClicked();
	void onScreenRecOkClicked();
private:
	Ui::ScreenRecordDialog *ui;
	WebSocket* webSocket;
	int& m_iResult;
};
