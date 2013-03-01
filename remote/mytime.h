#ifndef MYTIME_H
#define MYTIME_H
#include <QTime>
#include <QTimer>
#include <QLCDNumber>
#include <QWidget>
#include <QDebug>

class MyTime :public QLCDNumber
{
	Q_OBJECT
public:
	MyTime(int num,QWidget *parent = 0);
public slots:
	void show_time(void);
private:
	QTimer timer;
	QString string;
};
#endif
