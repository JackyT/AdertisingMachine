#include "vedio.h"
#include <QDir>
#include <QFile>
#include <iterator>

using namespace std;

	Vedio::Vedio(QWidget *parent)
:QWidget(parent)
{
}
Vedio::~Vedio()
{
	delete v_proc;
}
void Vedio::vedio_display(void)
{
	QStringList args;
	QStringList filter;
	filter << "*.mp4" << "*.flv" << "*.avi" << "*.rmvb";

	QDir dir(VEDIO_PATH);	
	vedio_list = dir.entryList(filter);
	QList<QString>::iterator It = vedio_list.begin();
	char list_path[] = VEDIO_LIST;
	QFile list(list_path);
	QTextStream out(&list); 
	if(!list.open(QIODevice::WriteOnly | QIODevice::Truncate)){
		qDebug() << "open file failed\n";
	}else{
		for(;It != vedio_list.end();It++){
			out << *It << endl;
			//qDebug() << *It;
		}
	}
	list.close();

	args << "-playlist" << list_path;
	args << "-loop" << "0" << "-wid" << QString::number(this->winId())
		<< "-x" << QString::number(this->width()) 
		<< "-y" << QString::number(this->height())
		<< "-zoom" << "-slave" << "-quiet";

	v_proc = new QProcess;
	v_proc->start("mplayer",args);
	//connect(v_proc, SIGNAL(started()), this, SLOT(readUnusedMsg()));
	//qDebug() << "run mplayer" << endl;

}
#if 0
void Vedio::readUnusedMsg()
{
	qDebug()  << "readUnusedMsg()" << endl;
	QTest::qWait(3000);//不要用QTest::qSleep()函数，它和qWait()不一样，会阻塞，不处理事件
	QByteArray msg;
	msg = v_proc->readAllStandardOutput();
	//qDebug()  << "Unused msg is: " << msg << endl;
	connect(v_proc, SIGNAL(readyReadStandardOutput()), this, SLOT(readFromMplayer()));
}
void Vedio::readFromMplayer()
{
	qDebug()  << "readFromMplayer()" << endl;
	QByteArray msg;
	msg = v_proc->readAllStandardOutput();
	//qDebug()  << "Answer is: " << msg << endl;
	if(parent_socket != NULL)
		parent_socket->write(msg);
	parent_socket = NULL;
}
#endif
void Vedio::rec_com(QByteArray str)
{
	v_proc->write(str);	
}
