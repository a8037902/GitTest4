#include "weblabel.h"


#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QSslConfiguration> 

#include "qrencode.h"



WebLabel::WebLabel(QWidget* parent)
	:QLabel(parent)
{
	//_pdataRecvWS = new QWebSocket();
	//_ptimer = new QTimer();

	/*QSslConfiguration config = _pdataRecvWS->sslConfiguration();
	config.setPeerVerifyMode(QSslSocket::VerifyNone);
	config.setProtocol(QSsl::TlsV1SslV3);
	_pdataRecvWS->setSslConfiguration(config);*/
	movie = new QMovie(":/powerdudemain/Resources/loading.gif");
	this->setMovie(movie);
	movie->start();

	pLogo = new QLabel(this);
	pLogo->setGeometry(QRect(220 / 2 - 20, 220 / 2 - 20, 40, 40));
	pLogo->setPixmap(QPixmap(":/powerdudemain/Resources/PowerBoy.ico"));
	pLogo->setScaledContents(true);
	pLogo->hide();
	
}



WebLabel::~WebLabel()
{
}

void WebLabel::stopMovie()
{
	pLogo->show();
	movie->stop();
	setPixmap(createQRCode(url));
}


void WebLabel::onShowUrl(QString url)
{
	pLogo->show();
	movie->stop();
	this->url = url;
	setPixmap(createQRCode(url));
}

QPixmap WebLabel::createQRCode(const QString &text)
{
	int margin = 2;
	if (text.length() == 0)
	{
		return QPixmap();
	}
	QRcode *qrcode = QRcode_encodeString(text.toLocal8Bit(), 2, QR_ECLEVEL_L, QR_MODE_8, 1);
	if (qrcode == NULL) {
		return QPixmap();
	}
	unsigned char *p, *q;
	p = NULL;
	q = NULL;
	int x, y, bit;
	int realwidth;

	realwidth = qrcode->width;
	QImage image = QImage(realwidth, realwidth, QImage::Format_Indexed8);
	QRgb value;
	value = qRgb(255, 255, 255);
	image.setColor(0, value);
	value = qRgb(0, 0, 0);
	image.setColor(1, value);
	image.setColor(2, value);
	image.fill(0);
	p = qrcode->data;
	for (y = 0; y < qrcode->width; y++) {
		bit = 7;
		q += margin / 8;
		bit = 7 - (margin % 8);
		for (x = 0; x < qrcode->width; x++) {
			if ((*p & 1) << bit)
				image.setPixel(x, y, 1);
			else
				image.setPixel(x, y, 0);
			bit--;
			if (bit < 0)
			{
				q++;
				bit = 7;
			}
			p++;
		}
	}
	return QPixmap::fromImage(image.scaledToWidth(220));
}

void WebLabel::onShowMovie()
{
	//movie->start();
	pLogo->hide();
	setPixmap(QPixmap());
	this->setMovie(movie);
	movie->start();
}

void WebLabel::onDisconnected()
{
	pLogo->hide();
	movie->stop();
	setPixmap(QPixmap(":/powerdudemain/Resources/invalid.png"));
}

void WebLabel::showEvent(QShowEvent * event)
{
	QLabel::showEvent(event);
}

void WebLabel::hideEvent(QHideEvent * event)
{
	//pLogo->show();
	//setPixmap(createQRCode(url));
	//movie->stop();
	QLabel::hideEvent(event);
}

void WebLabel::mousePressEvent(QMouseEvent *ev)
{
	//QWidget;;mousePressEvent(ev);
	qDebug() << ev->x() << ev->y();
}

void WebLabel::mouseReleaseEvent(QMouseEvent *ev)
{
	//QWidget;;mouseReleaseEvent(ev);
	//if (!pLogo->isHidden()) {
		onShowMovie();
		emit clicked();
	//}
}

void WebLabel::leaveEvent(QEvent *event)
{
	setMouseTracking(false);
	emit leave();
}

void WebLabel::enterEvent(QEvent *event)
{
	setMouseTracking(true);
	emit enter();
}

void WebLabel::mouseMoveEvent(QMouseEvent *event)
{
	//qDebug() << "Mouse " << cursor().pos().x() << " " << cursor().pos().y();
	emit mouseMove(cursor());
}