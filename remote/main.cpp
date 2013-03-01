#include <QApplication>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "mainwindow.h"
using namespace std;
int main(int argc, char *argv[])
{
	QApplication app(argc,argv);	
	//MainWindow mainwindow;
	//mainwindow.show();
#if 1 
	MainWindow *mainwindow;
	if(argc == 3){
		mainwindow = new MainWindow(argv[2]);
		mainwindow->showFullScreen();
	}else if(argc == 4){
		mainwindow = new MainWindow(argv[3]);
		mainwindow->setGeometry(0,0,atoi(argv[1]),atoi(argv[2]));		
		mainwindow->show();
	}else{
		mainwindow = new MainWindow(argv[1]);
		mainwindow->show();
	}
#endif
	return app.exec();
}	
