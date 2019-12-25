#include "screenrecorddialog.h"
#include "ui_screenrecorddialog.h"
#include <Windows.h>
#include "settings.h"
#include "OneCloudMessageBox.h"
#include <TlHelp32.h>

ScreenRecordDialog::ScreenRecordDialog(WebSocket* pWebSocket, int& iResult, QWidget *parent)
	: QDialog(parent),
	webSocket(pWebSocket),
	m_iResult(iResult)
{
	ui = new Ui::ScreenRecordDialog();
	ui->setupUi(this);

	Qt::WindowFlags flags =
		(windowFlags());
	setWindowFlags(Qt::FramelessWindowHint | Qt::Tool | Qt::Window);

	setAttribute(Qt::WA_TranslucentBackground);

	SingleTon<Settings>::getInstance().beginGroup("ScreenRecord");
	int quality = SingleTon<Settings>::getInstance().value("quality", 1).toInt();
	if (quality == 0) {
		ui->radioButton_low->setChecked(true);
	}
	else if (quality == 1) {
		ui->radioButton_middle->setChecked(true);
	}
	else {
		ui->radioButton_high->setChecked(true);
	}
	SingleTon<Settings>::getInstance().endGroup();

	connect(ui->pushButton_close, SIGNAL(clicked()), this, SLOT(onScreenRecCloseClicked()));
	connect(ui->pushButton_cancel, SIGNAL(clicked()), this, SLOT(onScreenRecCloseClicked()));
	connect(ui->pushButton_ok, SIGNAL(clicked()), this, SLOT(onScreenRecOkClicked()));
}

ScreenRecordDialog::~ScreenRecordDialog()
{
	delete ui;
}

void ScreenRecordDialog::paintEvent(QPaintEvent *event)
{
	QPainterPath path;
	path.setFillRule(Qt::WindingFill);
	path.addRect(5, 5, this->width() - 10, this->height() - 10);

	QPainter painter(this);
	painter.setRenderHint(QPainter::Antialiasing, true);
	painter.fillPath(path, QBrush(Qt::white));

	QColor color(0, 0, 0, 50);
	for (int i = 0; i < 5; i++)
	{
		QPainterPath path;
		path.setFillRule(Qt::WindingFill);
		path.addRect(5 - i, 5 - i, this->width() - (5 - i) * 2, this->height() - (5 - i) * 2);
		color.setAlpha(150 - qSqrt(i) * 50);
		painter.setPen(color);
		painter.drawPath(path);
	}
	QDialog::paintEvent(event);
}

bool ScreenRecordDialog::nativeEvent(const QByteArray &eventType, void *message, long *result)
{
	PMSG msg = (PMSG)message;
	if (msg->message != WM_NCHITTEST) {
		return QDialog::nativeEvent(eventType, message, result);
	}
	QPoint pos = mapFromGlobal(QCursor::pos());
	QWidget* pWidget = this->childAt(pos);
	if (pWidget != 0 && pWidget->inherits("QPushButton")) {
		return false;
	}
	if (pWidget != 0 && pWidget->inherits("QRadioButton")) {
		return false;
	}
	*result = HTCAPTION;
	return true;
}

void ScreenRecordDialog::onScreenRecCloseClicked()
{
	m_iResult = 0;
	this->reject();
}

static bool GetProcessByName(const wchar_t* strProcessName, DWORD& id)
{
	PROCESSENTRY32 pe = { 0 };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	pe.dwSize = sizeof(PROCESSENTRY32);
	if (!Process32First(hSnapshot, &pe)) {
		return false;
	}
	bool bExists = false;
	while (true) {
		pe.dwSize = sizeof(PROCESSENTRY32);
		if (Process32Next(hSnapshot, &pe) == FALSE) {
			break;
		}
		if (lstrcmpW(pe.szExeFile, strProcessName) == 0) {
			id = pe.th32ProcessID;
			bExists = true;
			break;
		}
	}
	CloseHandle(hSnapshot);
	return bExists;
}


void ScreenRecordDialog::onScreenRecOkClicked()
{
	int quality = 1;
	if (ui->radioButton_high->isChecked()) {
		quality = 2;
	}
	else if (ui->radioButton_middle->isChecked()) {
		quality = 1;
	}
	else {
		quality = 0;
	}
	bool isAutoOpen;
	SingleTon<Settings>::getInstance().beginGroup("File");
	isAutoOpen = SingleTon<Settings>::getInstance().value("fileOpen", false).toBool();
	SingleTon<Settings>::getInstance().endGroup();

	QString strFilePath = SingleTon<Settings>::getInstance().oneValue("File", "filePath", QStandardPaths::standardLocations(QStandardPaths::DownloadLocation)).toString();
	wchar_t strBuffer[MAX_PATH] = { 0 };
	wsprintfW(strBuffer, L"\"%s\\ScreenRecord.exe\"", QCoreApplication::applicationDirPath().toStdWString().c_str());
	wchar_t strParameters[MAX_PATH] = { 0 };
	if (webSocket->isLogin()) {
		wsprintfW(strParameters, L"%d \"%s\" %d %s %s", quality, strFilePath.toStdWString().c_str(), isAutoOpen?1:0, webSocket->m_chatSvrInfo.token.toStdWString().c_str(), webSocket->m_chatSvrInfo.chatServerApi.toStdWString().c_str());
	}
	else {
		wsprintfW(strParameters, L"%d \"%s\" %d", quality, strFilePath.toStdWString().c_str(),isAutoOpen ? 1 : 0);
	}
	m_iResult = 0;
	DWORD dwProcessId = -1;
	if (!GetProcessByName(L"ScreenRecord.exe", dwProcessId)) {
		SingleTon<Settings>::getInstance().setOneValue("ScreenRecord", "quality", quality);
		HINSTANCE hInstance = ::ShellExecuteW(0, L"open", strBuffer, strParameters, QCoreApplication::applicationDirPath().toStdWString().c_str(), SW_SHOWNORMAL);
		if (long(hInstance) <= 32) {
			m_iResult = -2;
		}
	}
	else {
		m_iResult = -1;
	}
	this->accept();
}