#include "mainwindow.h" 
#include <iostream>
#include <fstream>
#include <string.h>
using namespace std;

	MainWindow::MainWindow(char *ip,QWidget *parent)
:QWidget(parent)
{
	this->setWindowTitle(tr("Advertising Machine Client"));	
	setGeometry(0,0,840,480);
	int width = this->width();
	int height = this->height();
	socket = NULL;
	sendback = false;
	background = new QLabel(this);
	background_src = new QPixmap(BACK_GROUND);
	background->setPixmap(background_src->scaled(width,height));
	background->setGeometry(0,0,width,height);	

	//---------------------------------------------------
	//load time
	//---------------------------------------------------
	timeshow = new MyTime(8,this);
	//---------------------------------------------------
	//load picture
	//---------------------------------------------------
	picture1 = new Picture(1,this);
	picture2 = new Picture(2,this);

	//---------------------------------------------------
	//load text
	//---------------------------------------------------
	text = new TextPlace(this);
	//QString Qstr = QString::fromUtf8("旁白君来啦，这期为大家带来经典MTV欣赏！");
	//text->change_content(Qstr);
	text->setmsg();

	//---------------------------------------------------
	//load vedio
	//---------------------------------------------------
	vedio = new Vedio(this);
#if 1 
	QPalette plt = vedio->palette();
	plt.setColor(QPalette::Background, Qt::blue);
	vedio->setPalette(plt);
#endif
	vedio->setGeometry(0,0,width*0.75,height*0.85);	
	vedio->vedio_display();
	connect(vedio->v_proc, SIGNAL(started()), this, SLOT(readUnusedMsg()));
	//---------------------------------------------------
	//Tcp server	
	//---------------------------------------------------
	server.listen(QHostAddress(ip),9000);
	connect(&server,SIGNAL(newConnection()),this,SLOT(answerConnect()));
	//qDebug() << ip << endl;
}
MainWindow::~MainWindow()
{
	delete vedio;
	delete background_src;
	delete background;
	delete picture1;
	delete picture2;
	delete socket;
}
void MainWindow::answerConnect(void)
{
	qDebug() << "new connect";
	socket = server.nextPendingConnection();
	connect(socket,SIGNAL(readyRead()),this,SLOT(dataArrive()));
}
void MainWindow::dataArrive(void)
{
	char buff[128];
	while(socket->bytesAvailable()){
		dataRecv = socket->readAll();	
		qDebug() << "recv:" << dataRecv;
		if(dataRecv.compare("pause\n") == 0){
			strcpy(buff,"pause\n");	
			vedioCmd(buff);
		}else if(dataRecv.compare("get status") == 0){
			strcpy(buff,"get status");	
			sendback = true;
			vedio->v_proc->write("get_file_name\n");
			
		}else if(dataRecv.compare("speed_mult 3\n") == 0){
			strcpy(buff,"speed_mult 3\n");	
			vedioCmd(buff);
			socket->close();
		}else if(dataRecv.compare("speed_set 1\n") == 0){
			strcpy(buff,"speed_set 1\n");	
			vedioCmd(buff);
			socket->close();
		}else if(dataRecv.compare("vedio list") == 0){
			ifstream in(VEDIO_LIST,ios::in);
			if(!in.is_open()){
				cerr<<"open vedio list failed."	<< endl;
			}
			while(!in.eof()){
				in.getline(buff,sizeof(buff));	
				//qDebug() << buff;
				char *p = &buff[strlen(buff)];
				*p++ = '\n';
				*p = '\0';
				socket->write(buff);
			}
			in.close();
			socket->close();
		}
	}
}
void MainWindow::readUnusedMsg()
{
	qDebug()  << "readUnusedMsg()" << endl;
	QTest::qWait(3000);//不要用QTest::qSleep()函数，它和qWait()不一样，会阻塞，不处理事件
	QByteArray msg;
	msg = vedio->v_proc->readAllStandardOutput();
	//qDebug()  << "Unused msg is: " << msg << endl;
	connect(vedio->v_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromMplayer()));
}
void MainWindow::readFromMplayer()
{
	qDebug()  << "readFromMplayer()" << endl;
	QByteArray msg;
	msg = vedio->v_proc->readAllStandardOutput();
	//qDebug()  << "Answer is: " << msg << endl;
	if(sendback)
		socket->write(msg);
	sendback = false;
	socket->close();
}
void MainWindow::vedioCmd(char *cmd)
{
	QByteArray command(cmd);
	vedio->rec_com(command);
}

void MainWindow::resizeEvent(QResizeEvent *event)
{
	int width = this->width();
	int height = this->height();
	background->setPixmap(background_src->scaled(width,height));
	background->setGeometry(0,0,width,height);	
	vedio->setGeometry(0,0,width*0.75,height*0.85);	
	text->setGeometry(0, height * 0.86, width, height * 0.14);
	picture1->setGeometry(width * 0.76, height * 0.12, width * 0.23, height * 0.34);
	picture2->setGeometry(width * 0.76, height * 0.50, width * 0.23, height * 0.34);
	timeshow->setGeometry(width*0.755,height*0.005,width*0.24,height*0.1);

}
#if 0
void MainWindow::setIp(char *str)
{
	ip = str;
	qDebug() << ip;
}
#endif
