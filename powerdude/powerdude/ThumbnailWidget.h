#pragma once

#include <QWidget>
namespace Ui { class ThumbnailWidget; };

class ThumbnailWidget : public QWidget
{
	Q_OBJECT

public:
	ThumbnailWidget(QWidget *parent = Q_NULLPTR);
	~ThumbnailWidget();

private:
	Ui::ThumbnailWidget *ui;
};
