#pragma once
#include "autohideframe.h"
class RemoteDesktopFrame :
	public AutoHideFrame
{
	Q_OBJECT
public:
	explicit RemoteDesktopFrame(QWidget* parent = nullptr);
	~RemoteDesktopFrame();

protected:
	virtual void paintEvent(QPaintEvent*);
};

