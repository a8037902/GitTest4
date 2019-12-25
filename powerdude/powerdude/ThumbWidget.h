#pragma once
#include <qwidget.h>
#include <qlabel.h>
#include <QVBoxLayout>
#include <qpushbutton.h>
#include <qtimer.h>
#include <QMouseEvent>
#include <Windows.h>
#include <mutex>
#include <QListWidgetItem>

class ThumbWidget :
	public QWidget
{
	Q_OBJECT
public:
	ThumbWidget(HWND hWnd,QWidget* parent = nullptr);
	ThumbWidget(QWidget* pWidget, QWidget* parent = nullptr);
	virtual ~ThumbWidget();

	void SetTitle(QString& strTitle);
	void SetThumbPixmap(QPixmap& pixmap);
	bool StartThumb();
	bool StopThumb();
	HWND GetHwnd();
	HDC LockDC(HDC hdc);
	void UnlockDC();
	void SetItem(QListWidgetItem* pItem) {
		m_pItem = pItem;
	}

	QListWidgetItem * GetItem() {
		return m_pItem;
	}

	void SetHwnd(HWND hWnd) {
		m_hWnd = hWnd;
	}

	void SetWidget(QWidget* pWidget) {
		m_pWidget = pWidget;
	}


	void hideCloseBtn();
	QLabel*				m_pBGLabel;
private:
	void Init();
protected:
	virtual void mousePressEvent(QMouseEvent *event);
signals:
	void thumbCloseWidget(QString&,bool isAxWidget);
	void thumbOpenWidget(QString&, bool isAxWidget);

private:
	QVBoxLayout*		m_pVBoxLayout;
	QHBoxLayout*		m_pHBoxLayout;
	QLabel*				m_pTitleLabel;
	QPushButton*		m_pCloseButton;
	QLabel*				m_pThumbLabel;
	HWND				m_hWnd;
	QTimer*				m_pTimer;
	unsigned char*		m_pBuffer;
	bool                m_bIsRunning;
	QWidget*			m_pWidget;

	HDC					m_hdc;
	HBITMAP				m_hbitmap;
	std::mutex			m_mutex;
	QListWidgetItem*	m_pItem;
private slots:
	void onGrapDesktopProc();
};

