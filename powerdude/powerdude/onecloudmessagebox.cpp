#include "onecloudmessagebox.h"
#include <QPainter>
#include <QtMath>
#include "font.h"
#include <Windows.h>


OneCloudMessageBox::OneCloudMessageBox(QWidget *parent)
	: QDialog(parent)
	, ui(new Ui::Dialog())
{
	Qt::WindowFlags flags =
		(windowFlags());
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);

	ui->setupUi(this);

	ui->label_2->setFont(Font());
	ui->label_3->setFont(Font(20, true));
	ui->okBtn->setFont(Font());
	ui->cancelBtn->setFont(Font());

	connect(ui->okBtn, SIGNAL(clicked()), this, SLOT(accept()));
	connect(ui->cancelBtn, SIGNAL(clicked()), this, SLOT(reject()));
	connect(ui->xBtn, SIGNAL(clicked()), this, SLOT(reject()));
}


OneCloudMessageBox::~OneCloudMessageBox()
{
}

void OneCloudMessageBox::setText(QString & text)
{
	ui->label_2->setText(text);
}

void OneCloudMessageBox::setTitle(QString & text)
{
	ui->label_3->setText(text);
}

int OneCloudMessageBox::information(QWidget * parent, QString & text)
{
	OneCloudMessageBox box(parent);
	box.setText(text);
	return box.exec();
}

bool OneCloudMessageBox::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	PMSG msg = (PMSG)message;
	if (msg->message != WM_NCHITTEST) {
		return QDialog::nativeEvent(eventType, message, result);
	}
	QPoint pos = mapFromGlobal(QCursor::pos());
	QWidget* pWidget = this->childAt(pos);
	if (pWidget != 0 && pWidget->inherits("QPushButton")) {
		return false;
	}
	if (pWidget != 0 && pWidget->inherits("QRadioButton")) {
		return false;
	}
	*result = HTCAPTION;
	return true;
}

int OneCloudMessageBox::information(QWidget * parent, QString & title, QString & text)
{
	OneCloudMessageBox box(parent);
	box.setTitle(title);
	box.setText(text);
	return box.exec();
}

int OneCloudMessageBox::warning(QWidget * parent, QString & title, QString & text)
{
	OneCloudMessageBox box(parent);
	box.setTitle(title);
	box.setText(text);
	return box.exec();
}

void OneCloudMessageBox::paintEvent(QPaintEvent *e)
{

	QPainter painter(this);

	const qreal radius = 5;
	QRectF rect = QRect(0, 0, this->width()-1, this->height()-1);

	QPainterPath path;

	path.moveTo(rect.bottomRight() - QPointF(0, radius));
	path.lineTo(rect.topRight() + QPointF(0, radius));
	path.arcTo(QRectF(QPointF(rect.topRight() - QPointF(radius * 2, 0)), QSize(radius * 2, radius * 2)), 0, 90);
	path.lineTo(rect.topLeft() + QPointF(radius, 0));
	path.arcTo(QRectF(QPointF(rect.topLeft()), QSize(radius * 2, radius * 2)), 90, 90);
	path.lineTo(rect.bottomLeft() - QPointF(0, radius));
	path.arcTo(QRectF(QPointF(rect.bottomLeft() - QPointF(0, radius * 2)), QSize(radius * 2, radius * 2)), 180, 90);
	path.lineTo(rect.bottomLeft() + QPointF(radius, 0));
	path.arcTo(QRectF(QPointF(rect.bottomRight() - QPointF(radius * 2, radius * 2)), QSize(radius * 2, radius * 2)), 270, 90);

	painter.fillPath(path, Qt::white);

	int edgeWidth = 3;

	painter.setPen(QColor(73, 73, 73));
	painter.drawLine(QPoint(edgeWidth, this->height() - 50), QPoint(this->width()- edgeWidth, this->height() - 50));

	QColor color(0, 0, 0, 50);
	for (int i = 0; i < edgeWidth; i++)
	{
		QPainterPath path;
		path.setFillRule(Qt::WindingFill);
		path.addRect(edgeWidth - i, edgeWidth - i, this->width() - (edgeWidth - i) * 2, this->height() - (edgeWidth - i) * 2);
		color.setAlpha(150 - qSqrt(i) * 50);
		painter.setPen(color);
		painter.drawPath(path);
	}

	//QWidget::paintEvent(e);
}

void OneCloudMessageBox::on_okBtn_clicked()
{
	//close();
}

void OneCloudMessageBox::on_cancelBtn_clicked()
{
	//close();
}

void OneCloudMessageBox::on_xBtn_clicked()
{
	//close();
}
