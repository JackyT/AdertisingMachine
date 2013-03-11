#include "picture.h"

Picture::Picture(int index,QWidget *parent)
:QLabel(parent)
{
	int width = parent->width();
	int height = parent->height();
	if(index == 1){
		setGeometry(width * 0.75, height * 0.12, width * 0.23, height * 0.34);
		loadpicture(PICTURE1_PATH);	
	}else
	if(index == 2){
		setGeometry(width * 0.75, height * 0.50, width * 0.23, height * 0.34);
		loadpicture(PICTURE2_PATH);	
	}
#if 1 
	cur_show = 0;
	pic_timer.start(FLLUSH_TIME);
	dynamic = true;
	connect(&pic_timer,SIGNAL(timeout()),this,SLOT(showpicture()));
#endif
}
Picture::~Picture()
{
	for (int i = 0; i < numb; i++)
		delete pic_map[i];
	delete[] pic_map;
}
//read pictures from the path
void Picture::loadpicture(QString pic_path)
{
	QDir dir(pic_path);		
	QStringList filter;
	filter << "*.jpg" << "*.bmp" << "*.jpeg";
	pic_list = dir.entryList(filter);
	QList<QString>::iterator It = pic_list.begin();
	numb = 0;
	for(;It != pic_list.end();It++)
	{
		//qDebug() << *It;	
		numb++;
	}
	pic_map = new QPixmap *[numb];
	for(int i = 0;i < numb;i++)
	{
		pic_map[i] = new QPixmap(pic_path +"/"+ pic_list[i]);
	}

}
//pixmap the picture 
void Picture::showpicture(void)
{
	cur_show += 1;
	if(cur_show >= numb)
		cur_show = 0;
	setPixmap(pic_map[cur_show]->scaled(this->width(), this->height()));
}

void Picture::resizeEvent(QResizeEvent *event)	//大小改变触发
{
	if (cur_show >= numb)
		cur_show = 0;
	setPixmap(pic_map[cur_show]->scaled(this->width(), this->height()));
}
