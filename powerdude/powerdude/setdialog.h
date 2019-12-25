#pragma once
#include <qdialog.h>
#include "ui_setdialog.h"
#include <QSimpleUpdater.h>
#include <QButtonGroup>
#include "update.h"

static const QString DEFAULT_UPDATE_URL = "http://appcc.pispower.com/downloads/powerdude-new/zip/updates.json";

class SetDialog :
	public QDialog
{
	Q_OBJECT
public:
	explicit SetDialog(QWidget *parent = Q_NULLPTR);
	~SetDialog();

	QString mkMutiDir(const QString path);

private slots:
	void on_updateBtn_clicked();
	void on_selectDirBtn_clicked();
	void on_filePathBtn_clicked();
	void ontoggled(bool enable);
	void on_checkBox_clicked(bool);
	void on_checkBox_2_clicked(bool);
	void on_checkBox_3_clicked(bool);
	void on_fileOpenCBox_clicked(bool);
	void on_fontCBox_clicked(bool);
	void on_selfRunCBox_clicked(bool);
	void on_searchNearbyCBox_clicked(bool);

	void onGetVersion(QString &version);
	void onCurremtIndexChanged(int index);
	void onColorDepthIndexChanged(int index);
	void onDesktopSizeIndexChanged(int index);
	void onExitButtonClicked(int index);
	void onLockButtonClicked(int index);
	void onValueChanged(int);
private:
	Ui::SetDialog* ui;
	Update *pb_update;
	QSimpleUpdater* m_updater;
	//bool compare(const QString& x, const QString& y);
	QString jsonUrl;
	QButtonGroup * m_pGroup;
	QButtonGroup * m_pLockGroup;
};
