#ifndef VEDIO_H
#define VEDIO_H

#include <QWidget>
#include <QProcess>
#include <QDebug>
#include <QTest>
#include <QByteArray>
#include <QTcpSocket>
#define VEDIO_PATH "vedio"
#define VEDIO_LIST "vedio/playlist"
class Vedio :public QWidget
{
	Q_OBJECT
public:
	Vedio(QWidget *parent = 0);
	~Vedio();
	void vedio_display(void);
	void rec_com(QByteArray str);
	//void vedio_status(char *str,int size);
	QProcess *v_proc;
	//QTcpSocket *parent_socket;
public slots:
	//void readFromMplayer(void);
	//void readUnusedMsg(void);
private:
	QStringList vedio_list;
	int vedio_sum;
};

#endif
