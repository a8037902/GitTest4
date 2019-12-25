#include "ThumbnailWidget.h"
#include "ui_ThumbnailWidget.h"

ThumbnailWidget::ThumbnailWidget(QWidget *parent)
	: QWidget(parent)
{
	ui = new Ui::ThumbnailWidget();
	ui->setupUi(this);
}

ThumbnailWidget::~ThumbnailWidget()
{
	delete ui;
}
