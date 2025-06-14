#include "ImageClear.h"

ImageClear::ImageClear(QWidget* parent)
	: QWidget(parent)
	, ui(new Ui::ImageClearClass)
{
	ui->setupUi(this);


	day = 0;
	hours = 0;

	ui->timeEdit_Time->setDisplayFormat("HH:mm");
	// ui->timeEdit_Time->setTime(QTime::currentTime());
	trayIcon = new QSystemTrayIcon(this);
	trayIcon->setIcon(QApplication::style()->standardIcon(QStyle::SP_ComputerIcon));
	trayIcon->show();
	this->resize(460, 300);
	this->setFixedSize(this->size());
	this->setWindowFlags(windowFlags() & ~Qt::WindowMaximizeButtonHint);


	configPath = QCoreApplication::applicationDirPath() + QDir::separator() + "config.ini";
	fileWatcher = new QFileSystemWatcher(this);
	if (QFile::exists(configPath)) {
		fileWatcher->addPath(configPath);
	}

	connect(ui->pushButton_Path, &QPushButton::clicked, this, &ImageClear::on_pushButtonPath_clicked);
	connect(ui->pushButton_Exe, &QPushButton::clicked, this, &ImageClear::on_pushButtonExe_clicked);
	connect(ui->pushButton_App, &QPushButton::clicked, this, &ImageClear::on_pushButtonApp_clicked);
	connect(fileWatcher, &QFileSystemWatcher::fileChanged, this, &ImageClear::configChanged);

	loadConfig();

	ui->lineEdit_Day->setValidator(new QIntValidator(0, 999999, this));
	ui->lineEdit_Hour->setValidator(new QIntValidator(0, 999999, this));

	timer = new QTimer(this);
	connect(timer, &QTimer::timeout, this, &ImageClear::deleteTime);
	timer->start(60 * 1000);



	QMenu* trayMenu = new QMenu(this);
	QAction* showAction = new QAction("显示", this);
	connect(showAction, &QAction::triggered, this, [=]() {
		this->showNormal();
		this->activateWindow();
		});
	trayMenu->addAction(showAction);

	// 添加“退出”菜单项
	QAction* exitAction = new QAction("退出", this);
	connect(exitAction, &QAction::triggered, this, [=]() {
		qApp->quit();
		});
	trayMenu->addAction(exitAction);
	trayIcon->setContextMenu(trayMenu);




	connect(trayIcon, &QSystemTrayIcon::activated, this, [=](QSystemTrayIcon::ActivationReason reason) {
		if (reason == QSystemTrayIcon::Trigger || reason == QSystemTrayIcon::DoubleClick) {
			this->showNormal();
			this->activateWindow();
		}
		});

}

ImageClear::~ImageClear()
{
	delete ui;
}

void ImageClear::changeEvent(QEvent* event)
{
	if (event->type() == QEvent::WindowStateChange) {
		if (this->isMinimized()) {
			QTimer::singleShot(0, this, &QWidget::hide);
		}
	}
	QWidget::changeEvent(event);
}

void ImageClear::on_pushButtonPath_clicked()
{
	QString dir = QFileDialog::getExistingDirectory(this, tr(""), QString(), QFileDialog::ShowDirsOnly | QFileDialog::DontResolveSymlinks);
	if (!dir.isEmpty()) {
		ui->lineEdit_Path->setText(dir);
	}
}

void ImageClear::on_pushButtonApp_clicked()
{
	QString path = ui->lineEdit_Path->text();
	QString day = ui->lineEdit_Day->text();
	QString hour = ui->lineEdit_Hour->text();
	QTime time = ui->timeEdit_Time->time();
	this->deleteHour = time.hour();
	this->deleteMinute = time.minute();
	this->rootPath = path;
	this->day = day.toInt();
	this->hours = hour.toInt();
	this->day = day.toInt();
	this->hours = hour.toInt();

	QString exeDir = QCoreApplication::applicationDirPath();
	QString iniPath = exeDir + QDir::separator() + "config.ini";
	QSettings settings(iniPath, QSettings::IniFormat);
	settings.setValue("Setting/Path", path);
	settings.setValue("Setting/Time", time.toString("HH:mm"));
	settings.setValue("Setting/Day", day);
	settings.setValue("Setting/Hour", hour);
	settings.setValue("Setting/imgFormat", this->imgFormat);
	settings.setValue("Setting/isDelete", this->isDelet);

}

void ImageClear::loadConfig()
{
	bool ok;
	QString iniPath = QCoreApplication::applicationDirPath() + QDir::separator() + "config.ini";
	QSettings settings(iniPath, QSettings::IniFormat);
	QString path = settings.value("Setting/Path", "").toString();
	QString timestr = settings.value("Setting/Time", "00:00").toString();
	QString day = settings.value("Setting/Day", "30").toString();
	QString hour = settings.value("Setting/Hour", "0").toString();
	QTime time = QTime::fromString(timestr, "HH:mm");
	QString format = settings.value("Setting/imgFormat", "jpg jpeg png bmp gif tff").toString();
	QString isdelete = settings.value("Setting/isDelete", "0").toString();



	/*for (QString& ext : imgFormat) {
		ext = ext.trimmed().toLower();
	}*/



	if (!time.isValid()) {
		int hour = timestr.toInt();
		if (hour >= 0 && hour <= 23) {
			time = QTime(hour, 0);
		}
		else {
			time = QTime(0, 0);
		}
	}
	int dayInt = day.toInt(&ok);
	if (!ok) {
		day = "30";
	}
	ok = false;
	int hourInt = hour.toInt(&ok);
	if (!ok) {
		hour = "0";
	}
	ok = false;
	int deleteInt = isdelete.toInt(&ok);
	if (!ok || (deleteInt != 0 && deleteInt != 1)) {
		isdelete = "0";
	}

	this->deleteHour = time.hour();
	this->deleteMinute = time.minute();
	this->rootPath = path;
	this->day = day.toInt();
	this->hours = hour.toInt();
	this->imgFormat = format.split(QRegExp("\\s+"), QString::SkipEmptyParts);
	this->isDelet = isdelete.toInt();

	ui->lineEdit_Path->setText(path);
	ui->lineEdit_Day->setText(day);
	ui->lineEdit_Hour->setText(hour);
	ui->timeEdit_Time->setTime(time);
}



void ImageClear::configChanged(const QString& path)
{
	loadConfig();

	if (!fileWatcher->files().contains(path)) {
		if (QFile::exists(path)) {
			fileWatcher->addPath(path);
		}
	}
}




void ImageClear::deleteTime()
{
	QTime now = QTime::currentTime();
	QTime target(deleteHour, deleteMinute);

	if (now.hour() == target.hour() && now.minute() == target.minute()) {
		deleteImages();
	}
}


//void ImageClear::deleteImages()
//{
//    QString rootPath = ui->lineEdit_Path->text();
//    int day = ui->lineEdit_Day->text().toInt();
//    int hours = ui->lineEdit_Hour->text().toInt();
//    int time = day * 24 + hours;
//
//    qint64 secondsThreshold = static_cast<qint64>(time) * 3600;
//    QDateTime now = QDateTime::currentDateTime();
//    QStringList imageExtensions = { "jpg", "jpeg", "png", "bmp", "gif", "tiff", "webp" };
//
//    std::function<void(const QString&)> cleanDir = [&](const QString& dirPath) {
//        QDir dir(dirPath);
//        QFileInfoList subDirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
//        for (const QFileInfo& subDirInfo : subDirs) {
//            cleanDir(subDirInfo.absoluteFilePath());
//        }
//
//        QFileInfoList files = dir.entryInfoList(QDir::Files);
//        for (const QFileInfo& fileInfo : files) {
//            QString ext = fileInfo.suffix().toLower();
//            if (!imageExtensions.contains(ext))
//                continue;
//
//            qint64 secondsDiff = fileInfo.lastModified().secsTo(now);
//            if (secondsDiff > secondsThreshold) {
//                if (QFile::remove(fileInfo.absoluteFilePath())) {
//                    qDebug() << "Deleted image:" << fileInfo.absoluteFilePath();
//                }
//                else {
//                    qWarning() << "Failed to delete image:" << fileInfo.absoluteFilePath();
//                }
//            }
//        }
//
//        if (dirPath != rootPath && dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty()) {
//            if (QDir().rmdir(dirPath)) {
//                qDebug() << "Deleted empty directory:" << dirPath;
//            }
//            else {
//                qWarning() << "Failed to delete directory:" << dirPath;
//            }
//        }
//    };
//
//    cleanDir(rootPath);
//}


void ImageClear::deleteImages()
{
	if (isDelet = 0)
		return;
	int time = day * 24 + hours;
	if (time == 0)
		return;
	int second = static_cast<qint64>(time) * 3600;
	QDateTime now = QDateTime::currentDateTime();
	//qDebug() << "Selected hour:" << deleteHour;
	// qDebug() << "Selected minute:" << deleteHour;
	cleanDirectory(rootPath, rootPath, now, second);
}


void ImageClear::on_pushButtonExe_clicked()
{
	if (ui->lineEdit_Path->text().isEmpty()) {
		//QMessageBox::warning(this, "警告", "请输入路径");
		return;
	}
	if (ui->lineEdit_Day->text().isEmpty()) {
		day = 0;
	}
	if (ui->lineEdit_Hour->text().isEmpty()) {
		hours = 0;
	}
	rootPath = ui->lineEdit_Path->text();
	day = ui->lineEdit_Day->text().toInt();
	hours = ui->lineEdit_Hour->text().toInt();

	deleteImages();
}


void ImageClear::cleanDirectory(const QString& dirPath, const QString& rootPath, const QDateTime& now, int seconds)
{
	QDir dir(dirPath);
	QStringList imageExtensions = { "jpg", "jpeg", "png", "bmp", "gif","tff" };
	QFileInfoList subDirs = dir.entryInfoList(QDir::Dirs | QDir::NoDotAndDotDot);
	for (const QFileInfo& subDirInfo : subDirs) {
		cleanDirectory(subDirInfo.absoluteFilePath(), rootPath, now, seconds);
	}

	QFileInfoList files = dir.entryInfoList(QDir::Files);
	for (const QFileInfo& fileInfo : files) {
		QString ext = fileInfo.suffix().toLower();
		if (!imageExtensions.contains(ext))
			continue;

		//int secondsDiff = fileInfo.lastModified().secsTo(now);
		int secondsDiff = fileInfo.birthTime().secsTo(now);
		if (secondsDiff > seconds) {
			if (QFile::remove(fileInfo.absoluteFilePath())) {
				qDebug() << "del img:" << fileInfo.absoluteFilePath();
			}
		}
	}

	if (dirPath != rootPath && dir.entryList(QDir::AllEntries | QDir::NoDotAndDotDot).isEmpty()) {
		if (QDir().rmdir(dirPath)) {
			qDebug() << "del dir:" << dirPath;
		}
	}
}