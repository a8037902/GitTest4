#pragma once
#include <qpushbutton.h>
class TranslucentButton :
	public QPushButton
{
public:
	explicit TranslucentButton(QWidget *parent = 0);
	~TranslucentButton();

protected:
	// ���ƿ���
	void paintEvent(QPaintEvent *event) Q_DECL_OVERRIDE;
};

