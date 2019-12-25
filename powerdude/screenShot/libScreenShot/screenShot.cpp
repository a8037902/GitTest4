#include "screenShot.h"


screenShot::screenShot(QWidget *parent)
	: QWidget(parent), m_curHwnd(0), m_resizeType(None),m_screenShotStep(SelectArea)
	, m_pCtrlPanel(0), m_pDrawerPanel(0), m_pDrawable(0), m_bIsRenderPaint(false), m_pCurProperty(0), m_bKeyControlPressed(false)
{
	ui.setupUi(this);
	this->setAttribute(Qt::WA_DeleteOnClose);
	setMouseTracking(true);
	m_selectedRect.setX(0);
	m_selectedRect.setY(0);
	m_selectedRect.setRight(0);
	m_selectedRect.setBottom(0);
	m_hitPoint.setX(0);
	m_hitPoint.setY(0);
	m_screenPixmap = QGuiApplication::primaryScreen()->grabWindow(0);
	m_screenImage = m_screenPixmap.toImage();

	m_proRectangle.size.PenSizeEnum = PenSizeEnum::Middle;
	m_proRectangle.color = qRgba(255, 0, 0, 255);

	m_proEllipse.size.PenSizeEnum = PenSizeEnum::Middle;
	m_proEllipse.color = qRgba(255, 0, 0, 255);

	m_proArrow.size.PenSizeEnum = PenSizeEnum::Middle;
	m_proArrow.color = qRgba(255, 0, 0, 255);

	m_proBrush.size.PenSizeEnum = PenSizeEnum::Middle;
	m_proBrush.color = qRgba(255, 0, 0, 255);

	m_proTransparent.size.PenSizeEnum = PenSizeEnum::Middle;
	m_proTransparent.color = qRgba(255, 0, 0, 100);

	m_proMask.size.MaskSize = MaskSize::Mask_8;
	m_proMask.color = qRgba(255, 0, 0, 255);

	m_proText.size.PenSizeInt = 12;
	m_proText.color = qRgba(255, 0, 0, 255);
}

screenShot::~screenShot()
{
	while (!m_objects.empty()) {
		IDrawable* pDrawable = m_objects.back();
		m_objects.pop_back();
		delete pDrawable;
	}
	emit screenShotClosed(this);
}

BOOL CALLBACK screenShot::EnumWindowsProc(HWND hwnd, LPARAM lParam)
{
	screenShot* pThis = reinterpret_cast<screenShot*>(lParam);
	LONG windowStyle = ::GetWindowLong(hwnd, 0);
	if (0 != pThis && hwnd != 0 && ::IsWindowVisible(hwnd) && ::GetParent(hwnd) == 0) {
		RECT windowRect = { 0 };
		::GetWindowRect(hwnd,&windowRect);
		if (windowRect.left >= 0 && windowRect.top >= 0 && windowRect.right >= 0 && windowRect.bottom >= 0) {
			//printf("%x: %d %d %d %d \n", hwnd, windowRect.left, windowRect.top, windowRect.right, windowRect.bottom);
			pThis->m_hwndsMap.insert(hwnd, windowRect);
		}
	}
	return TRUE;
}

void screenShot::initScreenShot()
{
	::EnumWindows(&screenShot::EnumWindowsProc, reinterpret_cast<LPARAM>(this));
	m_pCtrlPanel = new QCtrlPanel(this);
	m_pCtrlPanel->hide();

	connect(m_pCtrlPanel->ui.toolButton_Cancel, SIGNAL(clicked()), this, SLOT(onCancelClicked()));
	connect(m_pCtrlPanel->ui.toolButton_OK, SIGNAL(clicked()), this, SLOT(onFinishClicked()));
	connect(m_pCtrlPanel->ui.toolButton_Save, SIGNAL(clicked()), this, SLOT(onSaveClicked()));
	connect(m_pCtrlPanel->ui.toolButton_undo, SIGNAL(clicked()), this, SLOT(onUndoClicked()));
	connect(m_pCtrlPanel->ui.radioButton_Rectangle, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonRectangleToggled(bool)));
	connect(m_pCtrlPanel->ui.radioButton_Arrow, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonArrowToggled(bool)));
	connect(m_pCtrlPanel->ui.radioButton_Brush, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonBrushToggled(bool)));
	connect(m_pCtrlPanel->ui.radioButton_Mask, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonMaskToggled(bool)));
	connect(m_pCtrlPanel->ui.radioButton_Text, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonTextToggled(bool)));
	connect(m_pCtrlPanel->ui.radioButton_Transparent, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonTransparentToggled(bool)));
	connect(m_pCtrlPanel->ui.radioButton_Elipse, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonElipseToggled(bool)));

	m_pDrawerPanel = new QDrawerPanel(this);
	m_pDrawerPanel->hide();

	ui.textEdit->hide();
	connect(ui.textEdit->document(), SIGNAL(contentsChanged()), this, SLOT(onTextAreaChanged()));
	ui.textEdit->setLineWrapMode(QTextEdit::NoWrap);
	ui.textEdit->setFontFamily("Microsoft YaHei");

	connect(m_pDrawerPanel->ui.color_11, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_12, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_21, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_22, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_31, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_32, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_41, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_42, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_51, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_52, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_61, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_62, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_71, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_72, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_81, SIGNAL(clicked()), this, SLOT(onColorSelected()));
	connect(m_pDrawerPanel->ui.color_82, SIGNAL(clicked()), this, SLOT(onColorSelected()));

	m_colorMaps.insert(m_pDrawerPanel->ui.color_11, qRgba(0,0,0,0xff));
	m_colorMaps.insert(m_pDrawerPanel->ui.color_12, qRgba(255, 255, 255, 0xff));

	m_colorMaps.insert(m_pDrawerPanel->ui.color_21, qRgba(132, 121, 157, 0xff));
	m_colorMaps.insert(m_pDrawerPanel->ui.color_22, qRgba(218, 218, 218, 0xff));

	m_colorMaps.insert(m_pDrawerPanel->ui.color_31, qRgba(85, 0, 0, 0xff));
	m_colorMaps.insert(m_pDrawerPanel->ui.color_32, qRgba(255, 0, 0, 0xff));

	m_colorMaps.insert(m_pDrawerPanel->ui.color_41, qRgba(170, 170, 0, 0xff));
	m_colorMaps.insert(m_pDrawerPanel->ui.color_42, qRgba(255, 255, 0, 0xff));

	m_colorMaps.insert(m_pDrawerPanel->ui.color_51, qRgba(0, 170, 0, 0xff));
	m_colorMaps.insert(m_pDrawerPanel->ui.color_52, qRgba(85, 255, 0, 0xff));

	m_colorMaps.insert(m_pDrawerPanel->ui.color_61, qRgba(0, 0, 127, 0xff));
	m_colorMaps.insert(m_pDrawerPanel->ui.color_62, qRgba(0, 0, 255, 0xff));

	m_colorMaps.insert(m_pDrawerPanel->ui.color_71, qRgba(85, 0, 127, 0xff));
	m_colorMaps.insert(m_pDrawerPanel->ui.color_72, qRgba(255, 0, 255, 0xff));

	m_colorMaps.insert(m_pDrawerPanel->ui.color_81, qRgba(0, 170, 127, 0xff));
	m_colorMaps.insert(m_pDrawerPanel->ui.color_82, qRgba(85, 255, 255, 0xff));

	connect(m_pDrawerPanel->ui.radioButton_pensize_small, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonPenSizeToggled(bool)));
	connect(m_pDrawerPanel->ui.radioButton_pensize_middle, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonPenSizeToggled(bool)));
	connect(m_pDrawerPanel->ui.radioButton_pensize_big, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonPenSizeToggled(bool)));

	connect(m_pDrawerPanel->ui.radioButton_degree_6, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonMaskSizeToggled(bool)));
	connect(m_pDrawerPanel->ui.radioButton_degree_8, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonMaskSizeToggled(bool)));
	connect(m_pDrawerPanel->ui.radioButton_degree_10, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonMaskSizeToggled(bool)));
	connect(m_pDrawerPanel->ui.radioButton_degree_12, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonMaskSizeToggled(bool)));
	connect(m_pDrawerPanel->ui.radioButton_degree_14, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonMaskSizeToggled(bool)));
	connect(m_pDrawerPanel->ui.radioButton_degree_16, SIGNAL(toggled(bool)), this, SLOT(onRadioButtonMaskSizeToggled(bool)));

	connect(m_pDrawerPanel->ui.comboBox_text_size, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));

	ui.titleLabel->setContentsMargins(8, 0, 8, 0);
	ui.line->setContentsMargins(8, 0, 8, 0);
	ui.infoLabel->setContentsMargins(8, 0, 8, 0);
	ui.scaleLabel->setContentsMargins(1, 1, 1, 0);

	POINT cursorPt = { 0 };
	::GetCursorPos(&cursorPt);
	QPoint point;
	point.setX(cursorPt.x);
	point.setY(cursorPt.y);
	updateDisplayInfo(point);
	this->showFullScreen();//将窗口部件全屏显示
}

bool screenShot::getWindowRect(int x, int y, LPRECT lpRect)
{
	bool bResult = false;
	QList<HWND> hwnds = m_hwndsMap.keys();
	for (QList<HWND>::iterator it = hwnds.begin(); it != hwnds.end(); it++) {
		if (m_hwndsMap.contains(*it)) {
			if (::PtInRect(&m_hwndsMap.value(*it), POINT{ x,y })) {
				*lpRect = m_hwndsMap.value(*it);
				bResult = true;
			}
		}
	}
	return bResult;
}

void screenShot::paintEvent(QPaintEvent *pPaintEvent)
{
	QPainter painter(this);
	painter.drawPixmap(0, 0, m_screenPixmap);
	if (!m_bIsRenderPaint) {//如果是最后渲染图片时 不要画其它图元。
		painter.save();
		painter.setRenderHint(QPainter::Antialiasing);
		QPainterPath screenPath;
		screenPath.moveTo(0, 0);
		screenPath.lineTo(m_screenPixmap.width(), 0);
		screenPath.lineTo(m_screenPixmap.width(), m_screenPixmap.height());
		screenPath.lineTo(0, m_screenPixmap.height());
		screenPath.lineTo(0, 0);

		if (screenShotStep::SelectArea == m_screenShotStep) {
			painter.setPen(QPen(Qt::red, 0, Qt::NoPen));
			painter.setBrush(QColor(0, 0, 0, 100));
			QPainterPath selectedPath;
			selectedPath.moveTo(m_selectedRect.left(), m_selectedRect.top());
			selectedPath.lineTo(m_selectedRect.right(), m_selectedRect.top());
			selectedPath.lineTo(m_selectedRect.right(), m_selectedRect.bottom());
			selectedPath.lineTo(m_selectedRect.left(), m_selectedRect.bottom());
			selectedPath.lineTo(m_selectedRect.left(), m_selectedRect.top());
			painter.drawPath(screenPath.subtracted(selectedPath));

			painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));
			painter.drawLine(m_selectedRect.left(), m_selectedRect.top(), m_selectedRect.right(), m_selectedRect.top());
			painter.drawLine(m_selectedRect.right(), m_selectedRect.top(), m_selectedRect.right(), m_selectedRect.bottom());
			painter.drawLine(m_selectedRect.right(), m_selectedRect.bottom(), m_selectedRect.left(), m_selectedRect.bottom());
			painter.drawLine(m_selectedRect.left(), m_selectedRect.bottom(), m_selectedRect.left(), m_selectedRect.top());

			painter.setBrush(Qt::red);

			painter.drawRect(m_left_top_rect);
			painter.drawRect(m_left_middle_rect);
			painter.drawRect(m_left_bottom_rect);

			painter.drawRect(m_center_top_rect);
			painter.drawRect(m_center_bottom_rect);

			painter.drawRect(m_right_top_rect);
			painter.drawRect(m_right_middle_rect);
			painter.drawRect(m_right_bottom_rect);
		}
		else {
			painter.setPen(QPen(Qt::red, 0, Qt::NoPen));
			painter.setBrush(QColor(0, 0, 0, 100));
			QPainterPath selectedPath;
			selectedPath.moveTo(m_selectedRect.left(), m_selectedRect.top());
			selectedPath.lineTo(m_selectedRect.right(), m_selectedRect.top());
			selectedPath.lineTo(m_selectedRect.right(), m_selectedRect.bottom());
			selectedPath.lineTo(m_selectedRect.left(), m_selectedRect.bottom());
			selectedPath.lineTo(m_selectedRect.left(), m_selectedRect.top());

			painter.drawPath(screenPath.subtracted(selectedPath));

			painter.setPen(QPen(Qt::red, 1, Qt::SolidLine));
			painter.drawLine(m_selectedRect.left(), m_selectedRect.top(), m_selectedRect.right(), m_selectedRect.top());
			painter.drawLine(m_selectedRect.right(), m_selectedRect.top(), m_selectedRect.right(), m_selectedRect.bottom());
			painter.drawLine(m_selectedRect.right(), m_selectedRect.bottom(), m_selectedRect.left(), m_selectedRect.bottom());
			painter.drawLine(m_selectedRect.left(), m_selectedRect.bottom(), m_selectedRect.left(), m_selectedRect.top());

			painter.setBrush(Qt::red);

			painter.drawRect(m_left_top_rect);
			painter.drawRect(m_left_middle_rect);
			painter.drawRect(m_left_bottom_rect);

			painter.drawRect(m_center_top_rect);
			painter.drawRect(m_center_bottom_rect);

			painter.drawRect(m_right_top_rect);
			painter.drawRect(m_right_middle_rect);
			painter.drawRect(m_right_bottom_rect);
		}
		painter.restore();
	}
	else {
		if (!m_pCtrlPanel->isHidden()) {
			m_pCtrlPanel->hide();
		}
		if (!m_pDrawerPanel->isHidden()) {
			m_pDrawerPanel->hide();
		}
		if (!ui.displayInfo->isHidden()) {
			ui.displayInfo->hide();
		}
	}
	for (QList<IDrawable*>::iterator i = m_objects.begin(); i != m_objects.end(); i++) {
		(*i)->Draw(painter);
	}
	QWidget::paintEvent(pPaintEvent);
}

void screenShot::updateRects(QPoint& startPoint, QPoint& endPoint)
{
	m_left_top_rect.setX(startPoint.x() - 2);
	m_left_top_rect.setY(startPoint.y() - 1);
	m_left_top_rect.setRight(startPoint.x() + 2);
	m_left_top_rect.setBottom(startPoint.y() + 2);

	m_center_top_rect.setX(0.5*(startPoint.x() + endPoint.x()) - 2);
	m_center_top_rect.setY(startPoint.y() - 1);
	m_center_top_rect.setRight(0.5*(startPoint.x() + endPoint.x()) + 2);
	m_center_top_rect.setBottom(startPoint.y() + 2);

	m_center_bottom_rect.setX(0.5*(startPoint.x() + endPoint.x()) - 2);
	m_center_bottom_rect.setY(endPoint.y() - 1);
	m_center_bottom_rect.setRight(0.5*(startPoint.x() + endPoint.x()) + 2);
	m_center_bottom_rect.setBottom(endPoint.y() + 2);

	m_right_top_rect.setX(endPoint.x() - 2);
	m_right_top_rect.setY(startPoint.y() - 1);
	m_right_top_rect.setRight(endPoint.x() + 2);
	m_right_top_rect.setBottom(startPoint.y() + 1);

	m_left_middle_rect.setX(startPoint.x() - 2);
	m_left_middle_rect.setY(0.5 * (startPoint.y() + endPoint.y()) - 1);
	m_left_middle_rect.setRight(startPoint.x() + 2);
	m_left_middle_rect.setBottom(0.5 * (startPoint.y() + endPoint.y()) + 2);

	m_right_middle_rect.setX(endPoint.x() - 2);
	m_right_middle_rect.setY(0.5 * (startPoint.y() + endPoint.y()) - 1);
	m_right_middle_rect.setRight(endPoint.x() + 2);
	m_right_middle_rect.setBottom(0.5 * (startPoint.y() + endPoint.y()) + 2);

	m_left_bottom_rect.setX(startPoint.x() - 2);
	m_left_bottom_rect.setY(endPoint.y() - 1);
	m_left_bottom_rect.setRight(startPoint.x() + 2);
	m_left_bottom_rect.setBottom(endPoint.y() + 2);

	m_right_bottom_rect.setX(endPoint.x() - 2);
	m_right_bottom_rect.setY(endPoint.y() - 1);
	m_right_bottom_rect.setRight(endPoint.x() + 2);
	m_right_bottom_rect.setBottom(endPoint.y() + 2);
}

void screenShot::updateDisplayInfo(QPoint& point)
{
	if (!ui.displayInfo->isHidden()) {
		QString strInfo;
		QRgb rgb = m_screenImage.pixel(point);
		strInfo.sprintf("%d x %d (%d , %d)\nRGB(%d %d %d)", abs(m_selectedRect.right() - m_selectedRect.left()), abs(m_selectedRect.bottom() - m_selectedRect.top()),
			point.x(), point.y(), qRed(rgb), qGreen(rgb), qBlue(rgb));
		ui.infoLabel->setText(strInfo);

		int pixmap_width = ui.scaleLabel->width() / 3;
		int pixmap_height = ui.scaleLabel->height() / 3;
		int pixmap_left = qMin(qMax(point.x() - pixmap_width / 2, 0),GetSystemMetrics(SM_CXSCREEN) - pixmap_width);
		int pixmap_top = qMin(qMax(point.y() - pixmap_height / 2, 0), GetSystemMetrics(SM_CYSCREEN) - pixmap_height);

		ui.scaleLabel->setPixmap(m_screenPixmap.copy(pixmap_left, pixmap_top, pixmap_width, pixmap_height));
		int x = point.x() + 20;
		int y = point.y() + 40;
		if (x + ui.displayInfo->width() > GetSystemMetrics(SM_CXSCREEN)) {
			x = point.x() - 20 - ui.displayInfo->width();
		}
		if (y + ui.displayInfo->height() > GetSystemMetrics(SM_CYSCREEN)) {
			y = point.y() - 40 - ui.displayInfo->height();
		}
		ui.displayInfo->setGeometry(x, y, ui.displayInfo->width(), ui.displayInfo->height());
	}
}

void screenShot::leftButtonPressedMouseMoveEvent(QMouseEvent *pMouseEvent)
{
	if (screenShotStep::SelectArea == m_screenShotStep) {
		m_selectedRect.setRight(pMouseEvent->pos().x());
		m_selectedRect.setBottom(pMouseEvent->pos().y());
		updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
		this->repaint();
	}
	else if (screenShotStep::ResizeArea == m_screenShotStep) {
		if (ResizeFD_LeftTop == m_resizeType) {
			m_selectedRect.setLeft(pMouseEvent->pos().x());
			m_selectedRect.setTop(pMouseEvent->pos().y());
			updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			this->repaint();
		}
		else if (ResizeFD_RightBottom == m_resizeType) {
			m_selectedRect.setRight(pMouseEvent->pos().x());
			m_selectedRect.setBottom(pMouseEvent->pos().y());
			updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			this->repaint();
		}
		else if (ResizeBD_RightTop == m_resizeType) {
			m_selectedRect.setRight(pMouseEvent->pos().x());
			m_selectedRect.setTop(pMouseEvent->pos().y());
			updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			this->repaint();
		}
		else if (ResizeBD_LeftBottom == m_resizeType) {
			m_selectedRect.setLeft(pMouseEvent->pos().x());
			m_selectedRect.setBottom(pMouseEvent->pos().y());
			updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			this->repaint();
		}
		else if (ResizeVer_Top == m_resizeType) {
			m_selectedRect.setTop(pMouseEvent->pos().y());
			updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			this->repaint();
		}
		else if (ResizeVer_Bottom == m_resizeType) {
			m_selectedRect.setBottom(pMouseEvent->pos().y());
			updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			this->repaint();
		}
		else if (ResizeHor_Left == m_resizeType) {
			m_selectedRect.setLeft(pMouseEvent->pos().x());
			updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			this->repaint();
		}
		else if (ResizeHor_Right == m_resizeType) {
			m_selectedRect.setRight(pMouseEvent->pos().x());
			updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			this->repaint();
		}
		else if (ResizeAll == m_resizeType) {
			QPoint vectorPoint = { pMouseEvent->pos().x() - m_hitPoint.x(),pMouseEvent->pos().y() - m_hitPoint.y() };
			QRect selectedRect;
			selectedRect.setLeft(m_selectedRect.left() + vectorPoint.x());
			selectedRect.setTop(m_selectedRect.top() + vectorPoint.y());
			selectedRect.setRight(m_selectedRect.right() + vectorPoint.x());
			selectedRect.setBottom(m_selectedRect.bottom() + vectorPoint.y());
			if (selectedRect.left() >= 0 && selectedRect.top() >= 0 && selectedRect.right() <= GetSystemMetrics(SM_CXSCREEN)
				&& selectedRect.bottom() <= GetSystemMetrics(SM_CYSCREEN)) {
				m_selectedRect = selectedRect;
			}
			m_hitPoint = pMouseEvent->pos();
			updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			this->repaint();
		}
		if (!m_pCtrlPanel->isHidden()) {
			m_pCtrlPanel->hide();
		}
		if (!m_pDrawerPanel->isHidden()) {
			m_pDrawerPanel->hide();
		}
	}
	else if (screenShotStep::EditRectangle == m_screenShotStep) {
		if (m_pDrawable != 0 && m_pDrawable->GetType() == DrawableType::RectangleObj && m_selectedRect.contains(pMouseEvent->pos())) {
			setCursor(Qt::CrossCursor);
			QRectangle* pRectangle = (QRectangle*)m_pDrawable;
			pRectangle->setEndPoint(pMouseEvent->pos());
			this->repaint();
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}
	else if (screenShotStep::EditEllipse == m_screenShotStep) {
		if (m_pDrawable != 0 && m_pDrawable->GetType() == DrawableType::EllipseObj && m_selectedRect.contains(pMouseEvent->pos())) {
			setCursor(Qt::CrossCursor);
			QEllipse* pEllipse = (QEllipse*)m_pDrawable;
			pEllipse->setEndPoint(pMouseEvent->pos());
			this->repaint();
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}
	else if (screenShotStep::EditArrow == m_screenShotStep) {
		if (m_pDrawable != 0 && m_pDrawable->GetType() == DrawableType::ArrowObj && m_selectedRect.contains(pMouseEvent->pos())) {
			setCursor(Qt::CrossCursor);
			QArrow* pArrow = (QArrow*)m_pDrawable;
			pArrow->setEndPoint(pMouseEvent->pos());
			this->repaint();
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}
	else if (screenShotStep::EditBrush == m_screenShotStep) {
		if (m_pDrawable != 0 && m_pDrawable->GetType() == DrawableType::BrushObj && m_selectedRect.contains(pMouseEvent->pos())) {
			setCursor(Qt::CrossCursor);
			QBrushDrawer* pBrush = (QBrushDrawer*)m_pDrawable;
			pBrush->addPoint(pMouseEvent->pos());
			this->repaint();
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}
	else if (screenShotStep::EditTransparent == m_screenShotStep) {
		if (m_pDrawable != 0 && m_pDrawable->GetType() == DrawableType::TransparentObj && m_selectedRect.contains(pMouseEvent->pos())) {
			setCursor(Qt::CrossCursor);
			QTransparent* pTransparent = (QTransparent*)m_pDrawable;
			pTransparent->addPoint(pMouseEvent->pos());
			this->repaint();
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}
	else if (screenShotStep::EditMask == m_screenShotStep) {
		if (m_pDrawable != 0 && m_pDrawable->GetType() == DrawableType::MaskObj && m_selectedRect.contains(pMouseEvent->pos())) {
			setCursor(Qt::CrossCursor);
			QMask* pMask = (QMask*)m_pDrawable;
			pMask->setEndPoint(pMouseEvent->pos());
			this->repaint();
		}
		else {
			setCursor(Qt::ArrowCursor);
		}
	}
}

void screenShot::leftButtonUnPressedMouseMoveEvent(QMouseEvent *pMouseEvent)
{
	if (screenShotStep::SelectArea == m_screenShotStep) {
		setCursor(Qt::ArrowCursor);
	}
	else if (screenShotStep::ResizeArea == m_screenShotStep) {
		if (m_left_top_rect.contains(pMouseEvent->pos())) {
			setCursor(Qt::SizeFDiagCursor);
			m_resizeType = ResizeFD_LeftTop;
		}
		else if (m_right_bottom_rect.contains(pMouseEvent->pos())) {
			setCursor(Qt::SizeFDiagCursor);
			m_resizeType = ResizeFD_RightBottom;
		}
		else if (m_right_top_rect.contains(pMouseEvent->pos())) {
			setCursor(Qt::SizeBDiagCursor);
			m_resizeType = ResizeBD_RightTop;
		}
		else if (m_left_bottom_rect.contains(pMouseEvent->pos())) {
			setCursor(Qt::SizeBDiagCursor);
			m_resizeType = ResizeBD_LeftBottom;
		}
		else if (m_center_top_rect.contains(pMouseEvent->pos())) {
			setCursor(Qt::SizeVerCursor);
			m_resizeType = ResizeVer_Top;
		}
		else if (m_center_bottom_rect.contains(pMouseEvent->pos())) {
			setCursor(Qt::SizeVerCursor);
			m_resizeType = ResizeVer_Bottom;
		}
		else if (m_left_middle_rect.contains(pMouseEvent->pos())) {
			setCursor(Qt::SizeHorCursor);
			m_resizeType = ResizeHor_Left;
		}
		else if (m_right_middle_rect.contains(pMouseEvent->pos())) {
			setCursor(Qt::SizeHorCursor);
			m_resizeType = ResizeHor_Right;
		}
		else if (m_selectedRect.contains(pMouseEvent->pos())) {
			setCursor(Qt::SizeAllCursor);
			m_resizeType = ResizeAll;
		}
		else {
			setCursor(Qt::ArrowCursor);
			m_resizeType = None;
		}
	}
	else {
		if (!m_selectedRect.contains(pMouseEvent->pos())) {
			setCursor(Qt::ArrowCursor);
		}
		else {
			setCursor(Qt::CrossCursor);
		}
	}
	if (!ui.infoLabel->isHidden()) {
		this->repaint();
	}
}

void screenShot::mouseDoubleClickEvent(QMouseEvent *pMouseEvent)
{
	if (pMouseEvent->button() & Qt::LeftButton && abs(m_selectedRect.right() - m_selectedRect.left()) > 0 &&
		abs(m_selectedRect.bottom() - m_selectedRect.top()) > 0 && m_selectedRect.contains(pMouseEvent->pos()) && m_screenShotStep == screenShotStep::ResizeArea){
		this->onFinishClicked();
	}
}

void screenShot::mouseMoveEvent(QMouseEvent *pMouseEvent)
{
	if (!m_pCtrlPanel->isHidden() && m_pCtrlPanel->geometry().contains(pMouseEvent->pos())) {
		setCursor(Qt::ArrowCursor);
		return;
	}
	else if (!m_pDrawerPanel->isHidden() && m_pDrawerPanel->geometry().contains(pMouseEvent->pos())) {
		setCursor(Qt::ArrowCursor);
		return;
	}
	updateDisplayInfo(pMouseEvent->pos());
	if (pMouseEvent->buttons() & Qt::LeftButton) {
		leftButtonPressedMouseMoveEvent(pMouseEvent);
	}
	else {
		leftButtonUnPressedMouseMoveEvent(pMouseEvent);
	}
}

void screenShot::mousePressEvent(QMouseEvent *pMouseEvent)
{
	if (screenShotStep::SelectArea == m_screenShotStep) {//如果是选择区域
		if (pMouseEvent->button() & Qt::LeftButton) {
			m_selectedRect.setLeft(pMouseEvent->pos().x());
			m_selectedRect.setTop(pMouseEvent->pos().y());
		}
	}
	else if (screenShotStep::ResizeArea == m_screenShotStep) {//如果是调整区域大小
		if (pMouseEvent->button() & Qt::LeftButton && resizeAreaType::ResizeAll == m_resizeType) {
			m_hitPoint = pMouseEvent->pos();
		}
	}
	else {
		if (!m_pCtrlPanel->isHidden() && m_pCtrlPanel->geometry().contains(pMouseEvent->pos()) || !m_pDrawerPanel->isHidden() && m_pDrawerPanel->geometry().contains(pMouseEvent->pos())) {
			return;
		}
		if (m_selectedRect.contains(pMouseEvent->pos())) {
			if (screenShotStep::EditRectangle == m_screenShotStep) {
				m_pDrawable = new QRectangle(pMouseEvent->pos(), m_proRectangle);
				m_objects.push_back(m_pDrawable);
			}
			else if (screenShotStep::EditEllipse == m_screenShotStep) {
				m_pDrawable = new QEllipse(pMouseEvent->pos(), m_proEllipse);
				m_objects.push_back(m_pDrawable);
			}
			else if (screenShotStep::EditArrow == m_screenShotStep) {
				m_pDrawable = new QArrow(pMouseEvent->pos(), m_proArrow);
				m_objects.push_back(m_pDrawable);
			}
			else if (screenShotStep::EditBrush == m_screenShotStep) {
				m_pDrawable = new QBrushDrawer(pMouseEvent->pos(), m_proBrush);
				m_objects.push_back(m_pDrawable);
			}
			else if (screenShotStep::EditTransparent == m_screenShotStep) {
				m_pDrawable = new QTransparent(pMouseEvent->pos(), m_proTransparent);
				m_objects.push_back(m_pDrawable);
			}
			else if (screenShotStep::EditMask == m_screenShotStep) {
				m_pDrawable = new QMask(pMouseEvent->pos(), m_proMask, &m_screenImage);
				m_objects.push_back(m_pDrawable);
			}
			else if (screenShotStep::EditText == m_screenShotStep) {
				if (ui.textEdit->isHidden()) {
					ui.textEdit->setGeometry(pMouseEvent->pos().x(), pMouseEvent->pos().y(), ui.textEdit->width(), ui.textEdit->height());
					ui.textEdit->setText("");
					ui.textEdit->setTextColor(m_proText.color);
					ui.textEdit->setFontPointSize(m_proText.size.PenSizeInt);
					ui.textEdit->show();
				}
				else {
					if (!ui.textEdit->toPlainText().isEmpty()) {
						QString strText = ui.textEdit->toPlainText();
						QRect pos = ui.textEdit->geometry();
						pos.setLeft(ui.textEdit->contentsMargins().left() + pos.left());
						pos.setTop(ui.textEdit->contentsMargins().top() + pos.top());
						m_objects.push_back(new QTextDrawer(pos, m_proText, strText));
					}
					ui.textEdit->setText("");
					ui.textEdit->hide();
				}
			}
		}
		else {
			setCursor(Qt::ArrowCursor);
			if (screenShotStep::EditText == m_screenShotStep) {
				if (!ui.textEdit->toPlainText().isEmpty()) {
					QString strText = ui.textEdit->toPlainText();
					QRect pos = ui.textEdit->geometry();
					pos.setLeft(ui.textEdit->contentsMargins().left() + pos.left());
					pos.setTop(ui.textEdit->contentsMargins().top() + pos.top());
					m_objects.push_back(new QTextDrawer(pos, m_proText, strText));
				}
				ui.textEdit->setText("");
				ui.textEdit->hide();
			}
		}
	}
}

void screenShot::mouseReleaseEvent(QMouseEvent *pMouseEvent)
{
	if (pMouseEvent->button() & Qt::LeftButton) {
		ui.displayInfo->hide();
		if (screenShotStep::SelectArea == m_screenShotStep) {
			m_screenShotStep = screenShotStep::ResizeArea;
			m_selectedRect.setRight(pMouseEvent->pos().x());
			m_selectedRect.setBottom(pMouseEvent->pos().y());
			if (m_selectedRect.left() > m_selectedRect.right()) {
				int left = m_selectedRect.left();
				m_selectedRect.setLeft(m_selectedRect.right());
				m_selectedRect.setRight(left);
			}
			if (m_selectedRect.top() > m_selectedRect.bottom()) {
				int top = m_selectedRect.top();
				m_selectedRect.setTop(m_selectedRect.bottom());
				m_selectedRect.setBottom(top);
			}
			updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			this->repaint();
			showCtrlPanel();
		}
		else if (screenShotStep::ResizeArea == m_screenShotStep) {
			showCtrlPanel();
		}
		else if (screenShotStep::EditMask == m_screenShotStep) {
			QMask* pMask = (QMask*)m_pDrawable;
			if (0 != pMask) {
				pMask->setDrawBorder(false);
			}
			m_pDrawable = 0;
			this->repaint();
		}
		else {
			m_pDrawable = 0;
		}
	}
	else if(pMouseEvent->button() & Qt::RightButton){
		if (screenShotStep::SelectArea == m_screenShotStep) {
			this->close();
		}
		else{
			m_selectedRect.setLeft(0);
			m_selectedRect.setRight(0);
			m_selectedRect.setTop(0);
			m_selectedRect.setBottom(0);
			this->updateRects(QPoint(m_selectedRect.left(), m_selectedRect.top()), QPoint(m_selectedRect.right(), m_selectedRect.bottom()));
			while (!m_objects.empty()) {
				IDrawable* pDrawable = m_objects.back();
				m_objects.pop_back();
				delete pDrawable;
			}
			m_pDrawable = 0;
			m_screenShotStep = screenShotStep::SelectArea;
			if (!m_pCtrlPanel->isHidden()) {
				m_pCtrlPanel->hide();
			}
			if (!m_pDrawerPanel->isHidden()) {
				m_pDrawerPanel->hide();
			}
			m_pCtrlPanel->ui.radioButton_Arrow->setCheckable(false);
			m_pCtrlPanel->ui.radioButton_Rectangle->setCheckable(false);
			m_pCtrlPanel->ui.radioButton_Elipse->setCheckable(false);
			m_pCtrlPanel->ui.radioButton_Brush->setCheckable(false);
			m_pCtrlPanel->ui.radioButton_Transparent->setCheckable(false);
			m_pCtrlPanel->ui.radioButton_Text->setCheckable(false);
			m_pCtrlPanel->ui.radioButton_Mask->setCheckable(false);

			m_pCtrlPanel->ui.radioButton_Arrow->setCheckable(true);
			m_pCtrlPanel->ui.radioButton_Rectangle->setCheckable(true);
			m_pCtrlPanel->ui.radioButton_Elipse->setCheckable(true);
			m_pCtrlPanel->ui.radioButton_Brush->setCheckable(true);
			m_pCtrlPanel->ui.radioButton_Transparent->setCheckable(true);
			m_pCtrlPanel->ui.radioButton_Text->setCheckable(true);
			m_pCtrlPanel->ui.radioButton_Mask->setCheckable(true);

			setCursor(Qt::ArrowCursor);
			ui.displayInfo->show();
			updateDisplayInfo(pMouseEvent->pos());
			this->repaint();
		}
	}
}

void screenShot::showCtrlPanel()
{
	if (m_screenShotStep == screenShotStep::SelectArea || screenShotStep::ResizeArea == m_screenShotStep) {
		if (m_selectedRect.bottom() + 8 + m_pCtrlPanel->height() <= GetSystemMetrics(SM_CYSCREEN)) {
			m_pCtrlPanel->setGeometry(max(m_selectedRect.right() - m_pCtrlPanel->width(), 0), m_selectedRect.bottom() + 8, m_pCtrlPanel->width(), m_pCtrlPanel->height());
		}
		else if (m_selectedRect.y() - 8 - m_pCtrlPanel->height() >= 0) {
			m_pCtrlPanel->setGeometry(max(m_selectedRect.right() - m_pCtrlPanel->width(), 0), m_selectedRect.y() - 8 - m_pCtrlPanel->height(), m_pCtrlPanel->width(), m_pCtrlPanel->height());
		}
		else {
			m_pCtrlPanel->setGeometry(max(m_selectedRect.right() - m_pCtrlPanel->width(), 0), m_selectedRect.bottom() - 8 - m_pCtrlPanel->height(), m_pCtrlPanel->width(), m_pCtrlPanel->height());
		}
		m_pCtrlPanel->displayResolution(abs(m_selectedRect.right() - m_selectedRect.left()), abs(m_selectedRect.bottom() - m_selectedRect.top()));
		m_pCtrlPanel->show();
		m_pDrawerPanel->hide();
	}
	else {
		if (m_selectedRect.bottom() + 16 + m_pCtrlPanel->height() + m_pDrawerPanel->height() <= GetSystemMetrics(SM_CYSCREEN)) {
			m_pCtrlPanel->setGeometry(max(m_selectedRect.right() - m_pCtrlPanel->width(), 0), m_selectedRect.bottom() + 8, m_pCtrlPanel->width(), m_pCtrlPanel->height());
			m_pDrawerPanel->setGeometry(max(m_selectedRect.right() - m_pCtrlPanel->width(), 0), m_selectedRect.bottom() + 16 + m_pCtrlPanel->height(), m_pDrawerPanel->width(), m_pDrawerPanel->height());
		}
		else if (m_selectedRect.y() - 16 - m_pCtrlPanel->height() - m_pDrawerPanel->height() >= 0) {
			m_pCtrlPanel->setGeometry(max(m_selectedRect.right() - m_pCtrlPanel->width(), 0), m_selectedRect.y() - 8 - m_pCtrlPanel->height(), m_pCtrlPanel->width(), m_pCtrlPanel->height());
			m_pDrawerPanel->setGeometry(max(m_selectedRect.right() - m_pCtrlPanel->width(), 0), m_selectedRect.y() - 16 - m_pCtrlPanel->height() - m_pDrawerPanel->height(), m_pDrawerPanel->width(), m_pDrawerPanel->height());
		}
		else {
			m_pCtrlPanel->setGeometry(max(m_selectedRect.right() - m_pCtrlPanel->width(), 0), m_selectedRect.bottom() - 8 - m_pCtrlPanel->height(), m_pCtrlPanel->width(), m_pCtrlPanel->height());
			m_pDrawerPanel->setGeometry(max(m_selectedRect.right() - m_pCtrlPanel->width(), 0), m_selectedRect.bottom() - 16 - m_pCtrlPanel->height() - m_pDrawerPanel->height(), m_pDrawerPanel->width(), m_pDrawerPanel->height());
		}
		m_pCtrlPanel->displayResolution(abs(m_selectedRect.right() - m_selectedRect.left()), abs(m_selectedRect.bottom() - m_selectedRect.top()));
		m_pCtrlPanel->show();
		m_pDrawerPanel->show();
	}
}

void screenShot::keyPressEvent(QKeyEvent *pKeyEvent)
{
	int key = pKeyEvent->key();
	switch (key){
		case Qt::Key_Escape:
			this->close();
			break;
		case Qt::Key_Enter:
			break;
		case Qt::Key_Return:
			break;
		case Qt::Key_Control:
			m_bKeyControlPressed = true;
			break;
		case Qt::Key_Z:
			if (m_bKeyControlPressed) {
				this->onUndoClicked();
			}
			break;
		case Qt::Key_S:
			if (m_bKeyControlPressed) {
				this->onSaveClicked();
			}
			break;
		default:
			break;
	}
}

void screenShot::keyReleaseEvent(QKeyEvent *pKeyEvent)
{
	int key = pKeyEvent->key();
	switch (key) {
		case Qt::Key_Control:
			m_bKeyControlPressed = false;
			break;
		case Qt::Key_Z:
			break;
		default:
			break;
	}
}

void screenShot::onCancelClicked()
{
	this->close();
}

void screenShot::onFinishClicked()
{
	ui.textEdit->hide();
	QClipboard *board = QApplication::clipboard();
	m_bIsRenderPaint = true;
	QPixmap pixmap = this->grab(QRect(m_selectedRect.left(), m_selectedRect.top(), m_selectedRect.right() - m_selectedRect.left(),
		m_selectedRect.bottom() - m_selectedRect.top()));
	m_bIsRenderPaint = false;
	board->setImage(pixmap.toImage());
	this->close();
}

void screenShot::onUndoClicked()
{
	if (!m_objects.empty()) {
		IDrawable* pDrawable = m_objects.back();
		m_objects.pop_back();
		delete pDrawable;
	}
	this->repaint();
}

void screenShot::onSaveClicked()
{
	if (m_screenShotStep == screenShotStep::SelectArea) {
		return;
	}
	QDateTime dateTime = QDateTime::currentDateTime();
	wchar_t strBuffer[MAX_PATH] = { 0 };
	swprintf(strBuffer,L"小智截图%d%d%d%d%d%d", dateTime.date().year(), dateTime.date().month(), dateTime.date().day(),
		dateTime.time().hour(), dateTime.time().minute(), dateTime.time().second());
	QString filename = QFileDialog::getSaveFileName(this, QStringLiteral("请选择要保存到的目录"), QString::fromWCharArray(strBuffer), "PNG Files(*.png);;JPEG Files(*.jpg);;BMP Files(*.bmp)");

	m_bIsRenderPaint = true;
	QPixmap pixmap = this->grab(QRect(m_selectedRect.left(), m_selectedRect.top(), m_selectedRect.right() - m_selectedRect.left(),
		m_selectedRect.bottom() - m_selectedRect.top()));
	m_bIsRenderPaint = false;
	if (pixmap.save(filename)) {
		this->close();
	}
	else {
		if (m_pCtrlPanel->isHidden()) {
			m_pCtrlPanel->show();
		}
		if (m_pDrawerPanel->isHidden() && m_screenShotStep != screenShotStep::ResizeArea) {
			m_pDrawerPanel->show();
		}
	}
}

void screenShot::onRadioButtonRectangleToggled(bool isToggled)
{
	if (isToggled) {
		m_pCurProperty = &m_proRectangle;
		if (m_pDrawerPanel->isHidden()) {
			m_pDrawerPanel->show();
		}
		m_pDrawerPanel->ui.stackedWidget->setCurrentIndex(0);
		m_pDrawerPanel->ui.stackedWidget_2->setCurrentIndex(0);
		m_screenShotStep = screenShotStep::EditRectangle;
		ui.textEdit->hide();
		if (m_proRectangle.size.PenSizeEnum == PenSizeEnum::Small) {
			m_pDrawerPanel->ui.radioButton_pensize_small->setChecked(true);
		}
		else if (m_proRectangle.size.PenSizeEnum == PenSizeEnum::Middle) {
			m_pDrawerPanel->ui.radioButton_pensize_middle->setChecked(true);
		}
		else if (m_proRectangle.size.PenSizeEnum == PenSizeEnum::Big) {
			m_pDrawerPanel->ui.radioButton_pensize_big->setChecked(true);
		}
		QToolButton* pToolButton = getToolButton(m_pCurProperty->color);
		if (pToolButton) {
			pToolButton->click();
		}
		showCtrlPanel();
	}
}

void screenShot::onRadioButtonElipseToggled(bool isToggled)
{
	if (isToggled) {
		m_pCurProperty = &m_proEllipse;
		if (m_pDrawerPanel->isHidden()) {
			m_pDrawerPanel->show();
		}
		m_pDrawerPanel->ui.stackedWidget->setCurrentIndex(0);
		m_pDrawerPanel->ui.stackedWidget_2->setCurrentIndex(0);
		m_screenShotStep = screenShotStep::EditEllipse;
		ui.textEdit->hide();
		if (m_proEllipse.size.PenSizeEnum == PenSizeEnum::Small) {
			m_pDrawerPanel->ui.radioButton_pensize_small->setChecked(true);
		}
		else if (m_proEllipse.size.PenSizeEnum == PenSizeEnum::Middle) {
			m_pDrawerPanel->ui.radioButton_pensize_middle->setChecked(true);
		}
		else if (m_proEllipse.size.PenSizeEnum == PenSizeEnum::Big) {
			m_pDrawerPanel->ui.radioButton_pensize_big->setChecked(true);
		}
		QToolButton* pToolButton = getToolButton(m_pCurProperty->color);
		if (pToolButton) {
			pToolButton->click();
		}
		showCtrlPanel();
	}
}

void screenShot::onRadioButtonBrushToggled(bool isToggled)
{
	if (isToggled) {
		m_pCurProperty = &m_proBrush;
		if (m_pDrawerPanel->isHidden()) {
			m_pDrawerPanel->show();
		}
		m_pDrawerPanel->ui.stackedWidget->setCurrentIndex(0);
		m_pDrawerPanel->ui.stackedWidget_2->setCurrentIndex(0);
		m_screenShotStep = screenShotStep::EditBrush;
		ui.textEdit->hide();
		if (m_proBrush.size.PenSizeEnum == PenSizeEnum::Small) {
			m_pDrawerPanel->ui.radioButton_pensize_small->setChecked(true);
		}
		else if (m_proBrush.size.PenSizeEnum == PenSizeEnum::Middle) {
			m_pDrawerPanel->ui.radioButton_pensize_middle->setChecked(true);
		}
		else if (m_proBrush.size.PenSizeEnum == PenSizeEnum::Big) {
			m_pDrawerPanel->ui.radioButton_pensize_big->setChecked(true);
		}
		QToolButton* pToolButton = getToolButton(m_pCurProperty->color);
		if (pToolButton) {
			pToolButton->click();
		}
		showCtrlPanel();
	}
}

void screenShot::onRadioButtonArrowToggled(bool isToggled)
{
	if (isToggled) {
		m_pCurProperty = &m_proArrow;
		if (m_pDrawerPanel->isHidden()) {
			m_pDrawerPanel->show();
		}
		m_pDrawerPanel->ui.stackedWidget->setCurrentIndex(0);
		m_pDrawerPanel->ui.stackedWidget_2->setCurrentIndex(0);
		m_screenShotStep = screenShotStep::EditArrow;
		ui.textEdit->hide();
		if (m_proArrow.size.PenSizeEnum == PenSizeEnum::Small) {
			m_pDrawerPanel->ui.radioButton_pensize_small->setChecked(true);
		}
		else if (m_proArrow.size.PenSizeEnum == PenSizeEnum::Middle) {
			m_pDrawerPanel->ui.radioButton_pensize_middle->setChecked(true);
		}
		else if (m_proArrow.size.PenSizeEnum == PenSizeEnum::Big) {
			m_pDrawerPanel->ui.radioButton_pensize_big->setChecked(true);
		}
		QToolButton* pToolButton = getToolButton(m_pCurProperty->color);
		if (pToolButton) {
			pToolButton->click();
		}
		showCtrlPanel();
	}
}

void screenShot::onRadioButtonMaskToggled(bool isToggled)
{
	if (isToggled) {
		m_pCurProperty = &m_proMask;
		if (m_pDrawerPanel->isHidden()) {
			m_pDrawerPanel->show();
		}
		m_pDrawerPanel->ui.stackedWidget->setCurrentIndex(1);
		m_screenShotStep = screenShotStep::EditMask;
		ui.textEdit->hide();
		if (m_proMask.size.MaskSize == MaskSize::Mask_6) {
			m_pDrawerPanel->ui.radioButton_degree_6->setChecked(true);
		}else if (m_proMask.size.MaskSize == MaskSize::Mask_8) {
			m_pDrawerPanel->ui.radioButton_degree_8->setChecked(true);
		}else if (m_proMask.size.MaskSize == MaskSize::Mask_10) {
			m_pDrawerPanel->ui.radioButton_degree_10->setChecked(true);
		}else if (m_proMask.size.MaskSize == MaskSize::Mask_12) {
			m_pDrawerPanel->ui.radioButton_degree_12->setChecked(true);
		}else if (m_proMask.size.MaskSize == MaskSize::Mask_14) {
			m_pDrawerPanel->ui.radioButton_degree_14->setChecked(true);
		}else if (m_proMask.size.MaskSize == MaskSize::Mask_16) {
			m_pDrawerPanel->ui.radioButton_degree_16->setChecked(true);
		}
		showCtrlPanel();
	}
}

void screenShot::onRadioButtonTextToggled(bool isToggled)
{
	if (isToggled) {
		m_pCurProperty = &m_proText;
		if (m_pDrawerPanel->isHidden()) {
			m_pDrawerPanel->show();
		}
		m_pDrawerPanel->ui.stackedWidget->setCurrentIndex(0);
		m_pDrawerPanel->ui.stackedWidget_2->setCurrentIndex(1);
		m_screenShotStep = screenShotStep::EditText;
		ui.textEdit->setFontPointSize(m_proText.size.PenSizeInt);
		QToolButton* pToolButton = getToolButton(m_pCurProperty->color);
		if (pToolButton) {
			pToolButton->click();
		}
		QString strText;
		strText.sprintf("%d", m_proText.size.PenSizeInt);
		m_pDrawerPanel->ui.comboBox_text_size->setCurrentText(strText);
		ui.textEdit->setTextColor(m_proText.color);
		showCtrlPanel();
	}
}

void screenShot::onRadioButtonTransparentToggled(bool isToggled)
{
	if (isToggled) {
		m_pCurProperty = &m_proTransparent;
		if (m_pDrawerPanel->isHidden()) {
			m_pDrawerPanel->show();
		}
		m_pDrawerPanel->ui.stackedWidget->setCurrentIndex(0);
		m_pDrawerPanel->ui.stackedWidget_2->setCurrentIndex(0);
		m_screenShotStep = screenShotStep::EditTransparent;
		ui.textEdit->hide();
		if (m_proTransparent.size.PenSizeEnum == PenSizeEnum::Small) {
			m_pDrawerPanel->ui.radioButton_pensize_small->setChecked(true);
		}
		else if (m_proTransparent.size.PenSizeEnum == PenSizeEnum::Middle) {
			m_pDrawerPanel->ui.radioButton_pensize_middle->setChecked(true);
		}
		else if (m_proTransparent.size.PenSizeEnum == PenSizeEnum::Big) {
			m_pDrawerPanel->ui.radioButton_pensize_big->setChecked(true);
		}
		QToolButton* pToolButton = getToolButton(m_pCurProperty->color);
		if (pToolButton) {
			pToolButton->click();
		}
		showCtrlPanel();
	}
}

void screenShot::onRadioButtonPenSizeToggled(bool isToggled)
{
	if (isToggled) {
		PenSizeEnum size;
		if (sender() == m_pDrawerPanel->ui.radioButton_pensize_small) {
			size = PenSizeEnum::Small;
		}else if (sender() == m_pDrawerPanel->ui.radioButton_pensize_middle) {
			size = PenSizeEnum::Middle;
		}
		else if (sender() == m_pDrawerPanel->ui.radioButton_pensize_big) {
			size = PenSizeEnum::Big;
		}
		if (m_pCurProperty != 0) {
			m_pCurProperty->size.PenSizeEnum = size;
		}
	}
}

void screenShot::onRadioButtonMaskSizeToggled(bool isToggled)
{
	if (isToggled) {
		MaskSize size;
		if (sender() == m_pDrawerPanel->ui.radioButton_degree_6) {
			size = MaskSize::Mask_6;
		}else if (sender() == m_pDrawerPanel->ui.radioButton_degree_8) {
			size = MaskSize::Mask_8;
		}else if (sender() == m_pDrawerPanel->ui.radioButton_degree_10) {
			size = MaskSize::Mask_10;
		}else if (sender() == m_pDrawerPanel->ui.radioButton_degree_12) {
			size = MaskSize::Mask_12;
		}else if (sender() == m_pDrawerPanel->ui.radioButton_degree_14) {
			size = MaskSize::Mask_14;
		}else if (sender() == m_pDrawerPanel->ui.radioButton_degree_16) {
			size = MaskSize::Mask_16;
		}
		if (m_pCurProperty != 0) {
			m_pCurProperty->size.MaskSize = size;
		}
	}
}

void screenShot::onTextAreaChanged()
{
	QTextDocument *document = qobject_cast<QTextDocument*>(sender());
	document->adjustSize();
	if (document){
		QTextEdit *editor = qobject_cast<QTextEdit*>(document->parent()->parent());
		if (editor){
			QRect pos = editor->geometry();
			int newwidth = document->size().width() + 10;
			if (pos.left() + newwidth > m_selectedRect.right()) {
				newwidth = m_selectedRect.right() - pos.left();
			}
			int newheight = document->size().height() + 20;
			if (pos.top() + newheight > m_selectedRect.bottom()) {
				newheight = m_selectedRect.bottom() - pos.top();
			}
			if (newwidth != editor->width()){
				editor->setFixedWidth(newwidth);
			}
			if (newheight != editor->height()){
				editor->setFixedHeight(newheight);
			}
		}
	}
}

void screenShot::onColorSelected()
{
	QToolButton* pToolButton = (QToolButton*)sender();
	if (0 != pToolButton) {
		m_pDrawerPanel->ui.label_color->setStyleSheet(pToolButton->styleSheet());
		if (m_colorMaps.contains(pToolButton)) {
			m_pCurProperty->color = m_colorMaps.value(pToolButton);
		}
		if (m_pCurProperty == &m_proText) {
			if (!ui.textEdit->isHidden() && !ui.textEdit->toPlainText().isEmpty()) {
				ui.textEdit->setFontPointSize(m_pCurProperty->size.PenSizeInt);
				ui.textEdit->setTextColor(m_pCurProperty->color);
			}
			else {
				ui.textEdit->setFontPointSize(m_pCurProperty->size.PenSizeInt);
				ui.textEdit->setTextColor(m_pCurProperty->color);
			}
		}
	}
}

void screenShot::onCurrentIndexChanged(int index)
{
	m_pCurProperty->size.PenSizeInt = m_pDrawerPanel->ui.comboBox_text_size->currentText().toInt();
	if (!ui.textEdit->isHidden() && !ui.textEdit->toPlainText().isEmpty()) {
		ui.textEdit->selectAll();
		ui.textEdit->setFontPointSize(m_pCurProperty->size.PenSizeInt);
		ui.textEdit->setTextColor(m_pCurProperty->color);
		ui.textEdit->undo();
	}
	else {
		ui.textEdit->setFontPointSize(m_pCurProperty->size.PenSizeInt);
		ui.textEdit->setTextColor(m_pCurProperty->color);
	}
}

QToolButton* screenShot::getToolButton(int color)
{
	QList<QToolButton*> keys = m_colorMaps.keys();
	for (QList<QToolButton*>::iterator it = keys.begin(); it != keys.end(); it++) {
		if (m_colorMaps.value((*it)) == color) {
			return (*it);
		}
	}
	return 0;
}