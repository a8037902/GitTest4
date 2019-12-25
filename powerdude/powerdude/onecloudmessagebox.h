#pragma once
#include<QDialog>
#include "ui_messagebox.h"
class OneCloudMessageBox :
	public QDialog
{
	Q_OBJECT
public:
	explicit OneCloudMessageBox(QWidget *parent = Q_NULLPTR);
	~OneCloudMessageBox();

	void setText(QString &text);
	void setTitle(QString &text);

	static int information(QWidget *parent, QString &text);
	static int information(QWidget *parent, QString &title, QString &text);
	static int warning(QWidget *parent, QString &title, QString &text);

protected:
	virtual void paintEvent(QPaintEvent*);
	virtual bool nativeEvent(const QByteArray &eventType, void *message, long *result);

private slots:
	void on_okBtn_clicked();
	void on_cancelBtn_clicked();
	void on_xBtn_clicked();

private:
	Ui::Dialog* ui;
};

