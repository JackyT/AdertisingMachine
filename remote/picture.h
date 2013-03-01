#ifndef PICTURE_H
#define PICTURE_H

#include <QTest>
#include <QDebug>
#include <QLabel>
#include <QTimer>
#include <QDir>
#include <iterator>
#define PICTURE1_PATH "picture1"
#define PICTURE2_PATH "picture2"

#define FLLUSH_TIME 3000

class Picture :public QLabel
{
	Q_OBJECT

public:
	Picture(int index,QWidget *parent = 0);
	~Picture();
	void loadpicture(QString pic_path);
	void resizeEvent(QResizeEvent *event);

public slots:
	void showpicture(void);

private:
	QPixmap **pic_map;
	QStringList pic_list;
	QTimer pic_timer;
	int numb;
	int cur_show;
	bool dynamic;
};

#endif
