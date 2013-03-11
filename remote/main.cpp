#include <QApplication>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include "mainwindow.h"
using namespace std;
int main(int argc, char *argv[])
{
	QApplication app(argc,argv);	
#if 1 
	MainWindow *mainwindow;
	if(argc == 3){ 
		//receive command open as fullsize
		mainwindow = new MainWindow(argv[2]);
		mainwindow->showFullScreen();
	}else if(argc == 4){
		//receive command open as size provided
		mainwindow = new MainWindow(argv[3]);
		mainwindow->setGeometry(0,0,atoi(argv[1]),atoi(argv[2]));		
		mainwindow->show();
	}else{
		//open normally
		mainwindow = new MainWindow(argv[1]);
		mainwindow->show();
	}
#endif
	return app.exec();
}	
