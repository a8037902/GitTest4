#include "tabicondialog.h"
#include "ui_tabicondialog.h"
#include <QApplication>
#include <QDesktopWidget>
#include "onecloudwidget.h"

TabIconDialog::TabIconDialog(pfBitBlt pfBitBlt, QWidget *parent)
	: QDialog(parent),
	m_pfBitBlt(pfBitBlt)
{
	ui = new Ui::TabIconDialog();
	ui->setupUi(this);

	Qt::WindowFlags flags =
		(windowFlags());
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::Window);

	setAttribute(Qt::WA_TranslucentBackground);

}

TabIconDialog::~TabIconDialog()
{
	foreach(ThumbWidget* var, mBtnList) {
		if (var) {
			var->deleteLater();
			var = NULL;
		}
	}
	mBtnList.clear();
	delete ui;
}

void TabIconDialog::paintEvent(QPaintEvent *event)
{
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	path.addRect(10, 10, this->width() - 20, this->height() - 20);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.fillPath(path, QBrush(QColor(0,0,0,200)));

	QColor color(0, 0, 0, 50);
	for (int i = 0; i < 10; i++)
	{
		QPainterPath path;
		path.setFillRule(Qt::WindingFill);
		path.addRect(10 - i, 10 - i, this->width() - (10 - i) * 2, this->height() - (10 - i) * 2);
		color.setAlpha(150 - qSqrt(i) * 50);
		painter.setPen(color);
		painter.drawPath(path);
	}
	QDialog::paintEvent(event);
}

void TabIconDialog::setWindowsList(QList<QWidget*> list, int index)
{
	foreach(ThumbWidget* var, mBtnList) {
		if (var) {
			var->deleteLater();
			var = NULL;
		}
	}
	mBtnList.clear();

	for (int i = list.size() - 1,j = 0; i >= 0; i--,j++) {
		ThumbWidget* pThumbWidget = 0;
		OneCloudWidget* pOneCloudWidget = qobject_cast<OneCloudWidget*>(list[i]);
		if (pOneCloudWidget != 0 && !pOneCloudWidget->getAxWidget()->isHidden()) {
			ThumbWidget* pThumb = (ThumbWidget*)pOneCloudWidget->getThumbWidget();
			pThumbWidget = new ThumbWidget(pOneCloudWidget->GetAxHwnd(), this);
			QString strTitle = QStringLiteral("[云桌面]") + pOneCloudWidget->whatsThis();
			pThumbWidget->SetTitle(strTitle);
			HDC hWndDC = pThumb != 0 ? pThumb->LockDC(0) : 0;
			if (hWndDC != 0) {
				HDC hdc = pThumbWidget->LockDC(hWndDC);
				RECT windowRect = { 0 };
				::GetWindowRect(pOneCloudWidget->GetAxHwnd(), &windowRect);
				m_pfBitBlt(hdc, 0, 0, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top, hWndDC, 0, 0, SRCCOPY);
				pThumbWidget->UnlockDC();
			}
			if (pThumb != 0) {
				pThumb->UnlockDC();
			}
		}
		else if (pOneCloudWidget != 0) {
			QString strTitle = QStringLiteral("[云桌面]") + pOneCloudWidget->whatsThis();
			pThumbWidget = new ThumbWidget(list[i], this);
			pThumbWidget->SetTitle(strTitle);
		}
		else {
			pThumbWidget = new ThumbWidget(list[i], this);
			pThumbWidget->SetTitle(list[i]->whatsThis());
		}
		pThumbWidget->hideCloseBtn();
		pThumbWidget->StartThumb();
		pThumbWidget->setWhatsThis(list[i]->whatsThis());
		mBtnList.push_back(pThumbWidget);
		ui->gridLayout->addWidget(pThumbWidget, j / 3, j % 3);
		if (i == index) {
			pThumbWidget->m_pBGLabel->setStyleSheet("background-color: rgb(160, 160, 160);border:2px solid white;");
		}
	}
	this->index = list.size() - 1 - index;
	if (!mBtnList.empty()) {
		showToGridLayout();
	}
}

QList<ThumbWidget*> TabIconDialog::getList()
{
	QList<ThumbWidget*> tempList;
	int iLastRow = ceil(mBtnList.count() / 3.0) - 1;
	int iSelectedRow = this->index / 3;
	int iStartRow = 0;
	if (iLastRow - iStartRow > 2) {//有需要剔除显示的情况
		iLastRow = iSelectedRow;
		if (iLastRow < 2) {
			iLastRow = 2;
		}
		if (iSelectedRow - iStartRow > 2) {
			iStartRow = iSelectedRow - 2;
		}
	}
	for (int i = 0; i < mBtnList.count(); i++) {
		int iRow = i / 3;
		if (iRow >= iStartRow && iRow <= iLastRow) {
			tempList.push_back(mBtnList[i]);
		}
	}
	return tempList;
}

void TabIconDialog::showToGridLayout()
{
	for (int i = 0; i < mBtnList.count(); i++) {
		mBtnList[i]->hide();
	}
	QList<ThumbWidget*> tempList = getList();
	for (int i = 0; i < tempList.count(); i++) {
		tempList[i]->show();
	}
	QMargins margins = ui->verticalLayout->layout()->contentsMargins();
	int spacing = ui->gridLayout->layout()->spacing();
	int row = 1;
	if (tempList.size() > 0) {
		row = int(ceil(tempList.size() / 3.0));
	}
	if (row > 3) {
		row = 3;
	}
	int h = margins.top() + margins.bottom() + row * 180 + row * spacing + 40;
	int w = margins.left() + margins.right() + 3 * (230 + spacing);
	this->resize(w, h);
}

void TabIconDialog::toNext()
{
	if (mBtnList.size() > 0) {
		mBtnList[index]->m_pBGLabel->setStyleSheet("background-color: rgb(160, 160, 160);");
		index = (++index) % mBtnList.size();
		mBtnList[index]->m_pBGLabel->setStyleSheet("background-color: rgb(160, 160, 160);border:2px solid white;");
		if (!mBtnList.empty()) {
			showToGridLayout();
		}
	}
}

ThumbWidget* TabIconDialog::getThumbWidget(HWND hWnd)
{
	ThumbWidget* pThumb = 0;
	for (int i = 0; i < mBtnList.count(); i++) {
		ThumbWidget* thumb = mBtnList[i];
		if (0 != thumb && thumb->GetHwnd() == hWnd) {
			pThumb = thumb;
			break;
		}
	}
	return pThumb;
}

void TabIconDialog::clearThumbList()
{
	foreach(ThumbWidget* var, mBtnList)
	{
		if (var) {
			var->StopThumb();
			var->deleteLater();
			var = NULL;
		}
	}
	mBtnList.clear();
}

void TabIconDialog::removeThumbWidget(ThumbWidget* pThumb)
{
	int iFound = -1;
	for (int i = 0; i < mBtnList.count(); i++) {
		if (pThumb == mBtnList[i]) {
			iFound = i;
			break;
		}
	}
	if (iFound > 0) {
		mBtnList.removeAt(iFound);
	}
}