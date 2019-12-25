#pragma once
#include <qwidget.h>
#include <QEvent.h>
#include "ui_infowidget.h"

class InfoWidget :
	public QWidget
{
	Q_OBJECT
public:
	explicit InfoWidget(QWidget *parent = Q_NULLPTR);
	~InfoWidget();

	enum ConnectStatus {
		Connecting = 0x1,
		Disconnecting = 0x2,
		ToConnecting = 0x3
	};

	void setStatusText(QString& text) {
		//this->ui->label_4->setText(text);
	}

	void setRelevance(bool b) {
		if (isRelevance != b) {
			isRelevance = b;
			if (b) {
				this->ui->label_4->setPixmap(QPixmap());
			}
			else {
				this->ui->label_4->setPixmap(QPixmap(":/infowidget/Resources/unconnection.png"));
			}
		}
	}

	bool getRelevance() {
		return isRelevance;
	}

	void setType(int type) {
		this->type = type;
		if (type != 0) {
			this->ui->label_4->setPixmap(QPixmap());
		}
		if (type == 0) {
			//this->ui->label_4->setPixmap(QPixmap());
			this->ui->label_2->setPixmap(QPixmap(":/infowidget/Resources/function_same_screen.png"));
		}else if (type == 1) {
			
			this->ui->label_2->setPixmap(QPixmap(":/infowidget/Resources/function_cloud_desktop.png"));
		}
		else if (type == 2) {
			this->ui->label_2->setPixmap(QPixmap(":/infowidget/Resources/list_p2p.png"));
		}
		else if (type == 8) {
			this->ui->label_2->setPixmap(QPixmap(":/infowidget/Resources/function_vnc.png"));
		}
		else if (type == 6) {
			this->ui->label_2->setPixmap(QPixmap(":/infowidget/Resources/function_ssh.png"));
		}
		else if (type == 40) {
			this->ui->label_2->setPixmap(QPixmap(":/infowidget/Resources/function_same_screen3.png"));
		}
		else if (type == 41) {
			this->ui->label_2->setPixmap(QPixmap(":/infowidget/Resources/function_same_screen2.png"));
		}
	}

	void hideX() {
		ui->conBtn->hide();
		ui->closeBtn->hide();
	}

	void showX() {
		ui->conBtn->show();
		ui->closeBtn->show();
	}

	bool isXBtnShow() {
		return !ui->closeBtn->isHidden();
	}

    void showCloseBtn() {
        ui->closeBtn->show();
    }

    void showConnBtn() {
        ui->conBtn->show();
    }

    void hideCloseBtn() {
        ui->closeBtn->hide();
    }

    void hideConnBtn() {
        ui->conBtn->hide();
    }

	int getType() {
		return type;
	}

	void setConnectStatus(ConnectStatus status) {
		if (status == Connecting) {
			//this->ui->label_4->setText(QStringLiteral("Á¬½ÓÖÐ"));
			this->ui->conBtn->setIcon(QIcon(":/infowidget/Resources/connecting.png"));
			//this->ui->closeBtn->show();
		}
		else if(status== Disconnecting)
		{
			//this->ui->label_4->setText(QStringLiteral(""));
			this->ui->conBtn->setIcon(QIcon(":/infowidget/Resources/disconnecting.png"));
			//this->ui->closeBtn->hide();
		}
		else if (status == ToConnecting)
		{
			this->ui->conBtn->setIcon(QIcon(":/infowidget/Resources/toconnecting.png"));
		}
		this->ui->conBtn->repaint();
		this->status = status;
	}

	void setText(QString& text) {
		this->ui->label_3->setText(text);
	}

	QString getText() {
		return this->ui->label_3->text();
	}

	void setName(QString& name) {
		this->ui->label->setText(name);
	}

	ConnectStatus getStatus() {
		return status;
	}

	void setKey(QString &key) {
		this->key = key;
	}

	QString getKey() {
		return key;
	}

signals:
	void clicked();
	void toConnect();
	void toDisconnect();
	void toClose();
	

private slots:
	void on_closeBtn_clicked();
	void on_conBtn_clicked();

protected:
	virtual void mousePressEvent(QMouseEvent *ev);
	virtual void mouseReleaseEvent(QMouseEvent *ev);

	virtual void paintEvent(QPaintEvent*);

private:
	Ui::InfoWidget* ui;
	ConnectStatus status;
	int type;
	QString key;
	bool isRelevance;
};

