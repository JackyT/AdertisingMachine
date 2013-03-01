#include "mytime.h"

MyTime::MyTime(int num,QWidget *parent)
:QLCDNumber(num,parent)
{
	int width = parent->width();
	int height = parent->height();

	string = QTime::currentTime().toString("hh:mm:ss");
	setGeometry(width*0.755,height*0.005,width*0.24,height*0.1);
	
	timer.start(1000);

	connect(&timer,SIGNAL(timeout()),this,SLOT(show_time()));
}

void MyTime::show_time(void)
{
	string = QTime::currentTime().toString("hh:mm:ss");
	display(string);
}
