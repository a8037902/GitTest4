#include "ThumbWidget.h"
/*dengpy*/
#include <rdpappcontainerclient.h>
#include <qScreen>
#include <qWindow>
#include <QTime>
/*dengpy*/

ThumbWidget::ThumbWidget(HWND hWnd,QWidget *parent)
	:QWidget(parent), m_hWnd(hWnd), m_pVBoxLayout(0), m_pTitleLabel(0), m_pHBoxLayout(0),
	m_pCloseButton(0), m_pThumbLabel(0), m_pTimer(0), m_pBuffer(0), m_bIsRunning(false), m_pWidget(0), m_hdc(0),
	m_hbitmap(0), m_pItem(0)
{
	m_pBuffer = (unsigned char*)malloc(1920 * 1080 * 4 + 1);
	Init();
}

ThumbWidget::ThumbWidget(QWidget* pWidget, QWidget* parent)
	:QWidget(parent), m_hWnd(0), m_pVBoxLayout(0), m_pTitleLabel(0), m_pHBoxLayout(0),
	m_pCloseButton(0), m_pThumbLabel(0), m_pTimer(0), m_pBuffer(0), m_bIsRunning(false), m_pWidget(pWidget), m_hdc(0),
	m_hbitmap(0), m_pItem(0)
{
	Init();
}


ThumbWidget::~ThumbWidget()
{
	this->StopThumb();
	if (m_pBuffer != 0) {
		free(m_pBuffer);
		m_pBuffer = 0;
	}
	if (m_hdc != 0) {
		::DeleteDC(m_hdc);
		m_hdc = 0;
	}
	if (m_hbitmap != 0) {
		::DeleteObject(m_hbitmap);
		m_hbitmap = 0;
	}
	if (m_pTimer != 0){
		delete m_pTimer;
		m_pTimer = 0;
	}
}


HWND ThumbWidget::GetHwnd()
{
	return m_hWnd;
}

void ThumbWidget::Init()
{
	this->setFixedWidth(222);
	this->setFixedHeight(172);

	m_pBGLabel = new QLabel(this);
	m_pBGLabel->setGeometry(0, 0, this->size().width(), this->size().height());

	m_pBGLabel->setStyleSheet("background-color: rgb(160, 160, 160);");

	m_pVBoxLayout = new QVBoxLayout(this);
	m_pHBoxLayout = new QHBoxLayout(this);
	m_pVBoxLayout->addLayout(m_pHBoxLayout);

	m_pTitleLabel = new QLabel(this);
	m_pTitleLabel->setStyleSheet("background-color: rgb(160, 160, 160); color: rgb(255, 255, 255);");

	m_pHBoxLayout->addWidget(m_pTitleLabel);

	m_pCloseButton = new QPushButton(this);
	m_pCloseButton->setFlat(true);
	m_pCloseButton->setFixedWidth(23);
	m_pCloseButton->setFixedHeight(23);

	m_pCloseButton->setStyleSheet("QPushButton{background-image: url(:/powerdudemain/Resources/closebtn.png);background-repeat: no-repeat;background-position:left;}"
		"QPushButton:hover{background-image: url(:/powerdudemain/Resources/closeHotBtn.png);background-repeat: no-repeat;background-position:left;}"
		"QPushButton:pressed{background-image: url(:/powerdudemain/Resources/closeHotBtn.png);background-repeat: no-repeat;background-position:left;}");
	m_pHBoxLayout->addWidget(m_pCloseButton);

	QObject::connect(m_pCloseButton, &QPushButton::clicked, [&] {
		emit thumbCloseWidget(this->whatsThis(), m_pWidget == 0?true:false);
	});

	m_pTimer = new QTimer();
	connect(m_pTimer, SIGNAL(timeout()), this, SLOT(onGrapDesktopProc()));
	m_pTimer->setSingleShot(true);

	m_pThumbLabel = new QLabel(this);
	m_pVBoxLayout->addWidget(m_pThumbLabel);
	m_pThumbLabel->setScaledContents(true);
}

void ThumbWidget::mousePressEvent(QMouseEvent *pMouseEvent)
{
	emit thumbOpenWidget(this->whatsThis(), m_pWidget == 0 ? true : false);
}

void ThumbWidget::SetTitle(QString& strTitle)
{
	m_pTitleLabel->setText(strTitle);
}

void ThumbWidget::SetThumbPixmap(QPixmap& pixmap)
{
	m_pThumbLabel->setPixmap(pixmap);
}

bool ThumbWidget::StartThumb()
{
	if ((m_pWidget == 0 && m_hWnd == 0) || m_bIsRunning) {
		return false;
	}
	m_bIsRunning = true;
	m_pTimer->start(2);
	return true;
}

bool ThumbWidget::StopThumb()
{
	if ((m_pWidget == 0 && m_hWnd == 0) || !m_bIsRunning) {
		return true;
	}
	m_bIsRunning = false;
	m_pTimer->stop();
	return true;
}

void ThumbWidget::onGrapDesktopProc()
{
	if (m_hWnd != 0 && m_bIsRunning) {
		if (m_hdc != 0) {
			BITMAP bitmap = { 0 };
			::GetObject(m_hbitmap, sizeof(BITMAP), &bitmap);
			BITMAPINFOHEADER bi = { 0 };
			CONST int nBitCount = 24;
			bi.biSize = sizeof(BITMAPINFOHEADER);
			bi.biWidth = bitmap.bmWidth;
			bi.biHeight = bitmap.bmHeight;
			bi.biPlanes = 1;
			bi.biBitCount = nBitCount;
			bi.biCompression = BI_RGB;
			DWORD dwSize = ((bitmap.bmWidth * nBitCount + 31) / 32) * 4 * bitmap.bmHeight;
			PBITMAPFILEHEADER pbf = (PBITMAPFILEHEADER)m_pBuffer;
			LPBITMAPINFOHEADER lpbi = (LPBITMAPINFOHEADER)(m_pBuffer + sizeof(BITMAPFILEHEADER));
			*lpbi = bi;
			m_mutex.lock();
			::GetDIBits(m_hdc, m_hbitmap, 0, bitmap.bmHeight, (BYTE*)lpbi + sizeof(BITMAPINFOHEADER), (BITMAPINFO*)lpbi, DIB_RGB_COLORS);
			m_mutex.unlock();
			pbf->bfType = 0x4d42;
			dwSize += sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			pbf->bfSize = dwSize;
			pbf->bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
			QPixmap pixmap;
			if (pixmap.loadFromData(m_pBuffer, sizeof(BITMAPFILEHEADER) + dwSize)) {
				this->SetThumbPixmap(pixmap);
			}
		}
		m_pTimer->start(50);
	}
	else if (m_pWidget != 0 && m_bIsRunning) {
		this->SetThumbPixmap(m_pWidget->grab());
		m_pTimer->start(50);
	}
	else {
		m_bIsRunning = false;
	}
}


HDC ThumbWidget::LockDC(HDC hdc)
{
	m_mutex.lock();
	if (m_hdc == 0 && hdc != 0 && m_hWnd != 0) {
		m_hdc = ::CreateCompatibleDC(hdc);
		RECT windowRect = { 0 };
		::GetWindowRect(m_hWnd, &windowRect);
		m_hbitmap = ::CreateCompatibleBitmap(hdc, windowRect.right - windowRect.left, windowRect.bottom - windowRect.top);
		::SelectObject(m_hdc, m_hbitmap);
	}
	return m_hdc;
}

void ThumbWidget::UnlockDC()
{
	m_mutex.unlock();
}

void ThumbWidget::hideCloseBtn()
{
	m_pCloseButton->hide();
}