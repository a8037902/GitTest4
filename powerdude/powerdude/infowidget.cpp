#include "infowidget.h"
#include <qdebug.h>
#include <QPainter>
#include <QBitmap>
#include "config.h"
#include "font.h"

InfoWidget::InfoWidget(QWidget *parent)
	: QWidget(parent)
	, ui(new Ui::InfoWidget())
	, status(Connecting)
	, isRelevance(false)
{
	ui->setupUi(this);

	//QPixmap pixmap(QString::fromUtf8(":/infowidget/Resources/unconnection.png"));
	//QBitmap mask = pixmap.createMaskFromColor(QColor(255, 255, 255), Qt::MaskInColor);
	//pixmap.setMask(mask);
	//QLabel *label_unconnection = ui->label_4;//new QLabel(this);
	//label_unconnection->setPixmap(pixmap);
	//label_unconnection->setScaledContents(true);
	//label_unconnection->setGeometry(0, 0, 18, 48);
	//label_unconnection->raise();
	//label_unconnection->setAttribute(Qt::WA_TranslucentBackground);

	//label_unconnection->setStyleSheet("background:transparent");


	QPalette pe;
	pe.setColor(QPalette::WindowText, Qt::black);
	ui->label->setPalette(pe);
	ui->label->setFont(Font(16, true));
	ui->label_3->setPalette(pe);
	ui->label_3->setFont(Font(12));
	//ui->label_4->setPalette(pe);
	ui->conBtn->setFont(Font(12));

}


InfoWidget::~InfoWidget()
{
}

void InfoWidget::paintEvent(QPaintEvent *e)
{

	QPainter painter(this);

	const qreal radius = 5;
	QRectF rect = QRect(0, 0, this->width(), this->height());

	/*QPainterPath path;

	path.moveTo(rect.bottomRight() - QPointF(0, radius));
	path.lineTo(rect.topRight() + QPointF(0, radius));
	path.arcTo(QRectF(QPointF(rect.topRight() - QPointF(radius * 2, 0)), QSize(radius * 2, radius * 2)), 0, 90);
	path.lineTo(rect.topLeft() + QPointF(radius, 0));
	path.arcTo(QRectF(QPointF(rect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
	path.lineTo(rect.bottomLeft() - QPointF(0, radius));
	path.arcTo(QRectF(QPointF(rect.bottomLeft() - QPointF(0, radius * 2)), QSize(radius * 2, radius * 2)), 180, 90);
	path.lineTo(rect.bottomLeft() + QPointF(radius, 0));
	path.arcTo(QRectF(QPointF(rect.bottomRight() - QPointF(radius * 2, radius * 2)), QSize(radius * 2, radius * 2)), 270, 90);

	painter.fillPath(path, QColor(Qt::white));*/
	painter.setPen(/*Qt::gray*/QColor(0xdd, 0xdd, 0xdd));
	painter.drawRect(rect);

	QWidget::paintEvent(e);
}

void InfoWidget::on_conBtn_clicked()
{
#ifdef ONE_CLOUD_USE
	if (status == Disconnecting) {
		//setConnectStatus(ToConnecting);
		emit toConnect();
	}
	else if (status == Connecting) {
		emit toDisconnect();
	}
#else
	emit toConnect();
#endif
}

void InfoWidget::mousePressEvent(QMouseEvent *ev)
{
	//QWidget;;mousePressEvent(ev);
	qDebug() << ev->x() << ev->y();
}

void InfoWidget::mouseReleaseEvent(QMouseEvent *ev)
{
	//QWidget;;mouseReleaseEvent(ev);
	emit clicked();
}

void InfoWidget::on_closeBtn_clicked()
{
	qDebug() << __FUNCTION__":" << __FILE__":" << __LINE__;
	emit toClose();
}

