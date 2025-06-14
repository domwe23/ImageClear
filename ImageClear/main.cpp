#include "ImageClear.h"
#include <QtWidgets/QApplication>
#include <QTextCodec>
#include <QApplication>
#include <QLocalSocket>
#include <QLocalServer>
#include <QFile>


bool isAlreadyRunning(const QString& key)
{
	QLocalSocket socket;
	socket.connectToServer(key);
	if (socket.waitForConnected(100)) {
		return true;
	}

	QLocalServer::removeServer(key);
	QLocalServer* server = new QLocalServer();
	server->listen(key);
	return false;
}




int main(int argc, char* argv[])
{

	QApplication app(argc, argv);
	QString uniqueKey = "ImageClearSingleInstance";
	if (isAlreadyRunning(uniqueKey)) {
		return 0;
	}
	ImageClear window;
	//window.show();
	//window.hide();
	return app.exec();
}
