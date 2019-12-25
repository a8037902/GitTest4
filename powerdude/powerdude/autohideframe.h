#pragma once
#include <qframe.h>
#include <QSize>
#include "DockWidget.h"
#include "DockMenu.h"
#include "ToolMenu.h"
#include <qtimer.h>

class AutoHideFrame :
	public QFrame
{
	Q_OBJECT

	Q_PROPERTY(bool autoHide READ isAutoHide WRITE setAutoHide)
	Q_PROPERTY(Driection hideDriection READ getHideDriection WRITE setHideDriection)
public:
	explicit AutoHideFrame(QWidget* parent = nullptr);
	~AutoHideFrame();

	enum Driection {
		Up = 0, // 
		Left = 0x0001, // 
		Right = 0x0002, // 
		Down = 0x0003, // 
		Centre = 0x0004,
	};
	Q_ENUM(Driection)

	bool isAutoHide() const;
	void setAutoHide(bool);
	Driection getHideDriection() const;
	void setHideDriection(Driection);

	void ToHideWidget();
	void hideWidget(bool isAnimate = true);
	void showWidget();

	void registerDockWidget(DockWidget* pDockWidget);

	void unRegisterDocketWidget();

	void setDockMenu(DockMenu* pDockMenu);

	void setToolMenu(ToolMenu* pToolMenu);

	void setAutoHideSize(QSize &size) {
		m_size = size;
	}

	int getShow() {
		return this->isShow;
	}

signals:
	void aboutTohide();
	void waitToHide();


private slots:
	void onHideFrame();
	void onKeepFrame();
protected:

	void leaveEvent(QEvent * event)override;
	void enterEvent(QEvent * event)override;
	//virtual void resizeEvent(QResizeEvent *event);

private:
	bool autoHide;
	int isShow;
	Driection hideDriection;
	QSize m_size;
	DockWidget*	m_pDockWidget;
	QTimer* m_pTimer;
	DockMenu*   m_pDockMenu;
	ToolMenu*   m_pToolMenu;
};

