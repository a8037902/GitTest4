#pragma once

#include "libscreenshot_global.h"
#include <QDialog>
#include "ui_ScreenShotDialog.h"
#include <Windows.h>
#include <QScreen>
#include <QPainter>
#include <QMouseEvent>
#include <QMap>
#include <QPoint>
#include <QList>
#include "QCtrlPanel.h"
#include "QDrawerPanel.h"
#include "IDrawable.h"
#include "QRectangle.h"
#include "QEllipse.h"
#include "QArrow.h"
#include "QBrush.h"
#include "QTransparent.h"
#include "QText.h"
#include "QMask.h"
#include <QDateTime>
#include <QFileDialog>
#include <QClipboard>
#include <QRgb>
#include <QTextDocument>
#include <QTextEdit>

class LIBSCREENSHOT_EXPORT ScreenShotDialog : public QDialog
{
	Q_OBJECT

public:
	ScreenShotDialog(QWidget *parent = Q_NULLPTR);
	~ScreenShotDialog();
	void initScreenShot();

	enum screenShotStep {
		SelectArea = 0,
		ResizeArea = 1,
		EditRectangle = 2,
		EditEllipse = 3,
		EditArrow = 4,
		EditBrush = 5,
		EditTransparent = 6,
		EditMask = 7,
		EditText = 8
	};

	enum resizeAreaType {
		None = 0,
		ResizeFD_LeftTop = 1,
		ResizeFD_RightBottom = 2,
		ResizeBD_RightTop = 3,
		ResizeBD_LeftBottom = 4,
		ResizeVer_Top = 5,
		ResizeVer_Bottom = 6,
		ResizeHor_Left = 7,
		ResizeHor_Right = 8,
		ResizeAll = 9
	};

public:
	void paintEvent(QPaintEvent *pPaintEvent);
	void mouseMoveEvent(QMouseEvent *pMouseEvent);
	void leftButtonPressedMouseMoveEvent(QMouseEvent *pMouseEvent);
	void leftButtonUnPressedMouseMoveEvent(QMouseEvent *pMouseEvent);
	void mousePressEvent(QMouseEvent *pMouseEvent);
	void mouseReleaseEvent(QMouseEvent *pMouseEvent);
	void keyPressEvent(QKeyEvent *pKeyEvent);
	void keyReleaseEvent(QKeyEvent *pKeyEvent);
	void mouseDoubleClickEvent(QMouseEvent *pMouseEvent);
public:
	static BOOL CALLBACK EnumWindowsProc(HWND hwnd, LPARAM lParam);

private:
	QPixmap		  m_screenPixmap;
	QImage		  m_screenImage;
	HWND		  m_curHwnd;
	QMap<HWND, RECT>   m_hwndsMap;
	QRect		  m_selectedRect;

	QRect		  m_left_top_rect;
	QRect		  m_left_middle_rect;
	QRect	      m_left_bottom_rect;

	QRect         m_center_top_rect;
	QRect		  m_center_bottom_rect;

	QRect         m_right_top_rect;
	QRect         m_right_middle_rect;
	QRect         m_right_bottom_rect;

	resizeAreaType m_resizeType;
	screenShotStep m_screenShotStep;
	QPoint        m_hitPoint;
	QCtrlPanel*	  m_pCtrlPanel;
	QDrawerPanel* m_pDrawerPanel;

	QList<IDrawable*> m_objects;
	IDrawable*	  m_pDrawable;
	bool		  m_bIsRenderPaint;

	DrawableProperty m_proRectangle;
	DrawableProperty m_proEllipse;
	DrawableProperty m_proArrow;
	DrawableProperty m_proBrush;
	DrawableProperty m_proTransparent;
	DrawableProperty m_proMask;
	DrawableProperty m_proText;

	DrawableProperty* m_pCurProperty;

	QMap<QToolButton*, QRgb> m_colorMaps;

	QToolButton* getToolButton(int color);
	bool getWindowRect(int x, int y, LPRECT lpRect);
	bool m_bKeyControlPressed;

private:
	void updateRects(QPoint& startPoint, QPoint& endPoint);
	void showCtrlPanel();
	void updateDisplayInfo(QPoint& point);

public slots:
	void onCancelClicked();
	void onFinishClicked();
	void onUndoClicked();
	void onSaveClicked();
	void onRadioButtonRectangleToggled(bool isToggled);
	void onRadioButtonElipseToggled(bool isToggled);
	void onRadioButtonBrushToggled(bool isToggled);
	void onRadioButtonArrowToggled(bool isToggled);
	void onRadioButtonMaskToggled(bool isToggled);
	void onRadioButtonTextToggled(bool isToggled);
	void onRadioButtonTransparentToggled(bool isToggled);
	void onRadioButtonPenSizeToggled(bool isToggled);
	void onRadioButtonMaskSizeToggled(bool isToggled);
	void onTextAreaChanged();
	void onColorSelected();
	void onCurrentIndexChanged(int index);

private:
	Ui::ScreenShotDialog ui;
};
