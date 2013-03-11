#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QLabel>
#include <QWidget>
#include <QPalette>
#include <QProcess>
#include <QByteArray>
#include <QTimer>
#include <QDebug>
#include <QHostAddress>
#include <QTcpServer>
#include <QTcpSocket>
#include <QTest>

#include "vedio.h"
#include "picture.h"
#include "textplace.h"
#include "mytime.h"

#define BACK_GROUND "doc/background.jpg"

class MainWindow :public QWidget
{
	Q_OBJECT
public:
	MainWindow(char *ip,QWidget *parent = 0);
	~MainWindow();
	void resizeEvent(QResizeEvent *event);
public slots:
	void answerConnect(void);
	void dataArrive(void);
	void readUnusedMsg(void); //read the unused mesage
	void readFromMplayer(void);
private:
	QLabel *background;
	QPixmap *background_src;
	Vedio *vedio;
	TextPlace *text;
	Picture *picture1;
	Picture *picture2;
	MyTime *timeshow;
	QTcpServer server;
	QTcpSocket *socket;
	QString dataRecv;

	bool sendback;
};

#endif
