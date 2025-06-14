#pragma once
#include <QtWidgets/QWidget>
#include "ui_ImageClear.h"
#include <QFileDialog>
#include <QDirIterator>
#include <QFileInfo>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QMessageBox>
#include <QSystemTrayIcon>
#include <QApplication>
#include <QStyle>
#include <QTimeEdit>
#include <QSettings>
#include <QFileSystemWatcher>
#include <QMenu>
#include <QMenu>
#pragma execution_character_set("utf-8")



class ImageClear : public QWidget
{
	Q_OBJECT

public:
	ImageClear(QWidget* parent = nullptr);
	~ImageClear();
	void cleanDirectory(const QString& dirPath, const QString& rootPath, const QDateTime& now, int seconds);
	void deleteImages();
	void loadConfig();

private slots:
	void on_pushButtonPath_clicked();
	void on_pushButtonExe_clicked();
	void on_pushButtonApp_clicked();
	void changeEvent(QEvent* event);
	void configChanged(const QString& path);
	void deleteTime();

private:
	//Ui::ImageClearClass ui;
	Ui::ImageClearClass* ui;
	QString rootPath;
	int day;
	int hours;
	int deleteHour;
	int deleteMinute;
	QTimer* timer;
	QSystemTrayIcon* trayIcon;
	QFileSystemWatcher* fileWatcher;
	QString configPath;
	//QStringList imgFormat = { "jpg", "jpeg", "png", "bmp", "gif","tff" };
	QStringList imgFormat;
	int isDelet;
};

