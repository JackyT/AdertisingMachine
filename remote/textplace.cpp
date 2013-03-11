#include "textplace.h"
#include <QPalette>
#include <QColor>
#include <QDebug>
#include <QFile>

TextPlace::TextPlace(QWidget *parent)
:QLabel(parent)
{
	setGeometry(0, parent->height()*0.86, parent->width(), parent->height()*0.14);
	QPalette plt;
	text = new QLabel(parent);				
	plt = text->palette();					
	text_color.setRgb(0xff,0x88,0x88, 255);		
	plt.setColor(QPalette::WindowText, text_color);
	text->setPalette(plt);
	text_family = "AR PL UKai CN";			

	text_size = height() * 0.4;			
	//qDebug() << "text_size=" << text_size;
	text->setFont(QFont(QString::fromUtf8(text_family), text_size));	
	texttimer = new QTimer(this);					
	texttimer->start(300);						
	connect(texttimer, SIGNAL(timeout()), this, SLOT(dynamic_displaytext()));
	i = width();									
	isshow = true;			
	isdynamic = true;	
	smoothness = 3;		
}

void TextPlace::setmsg()		//read the text display file
{
	QString msg;
	QFile textfile(TEXT_DISPLAY);
	if(!textfile.open(QIODevice::ReadOnly)){
		qDebug() << "open doc/textdisplay failed";
		exit(2);
	}else{
		QTextStream in(&textfile);		
		msg = in.readAll();	
	}
	textfile.close();	
	text->setText(msg);
	textwidth = text->fontMetrics().width(text->text()) * 1;
}
void TextPlace::change_content(QString str) //change the file content,program read this file's content as the text
{
	qDebug() << "set content:" << str;
	QFile textfile(TEXT_DISPLAY);
	if(!textfile.open(QIODevice::WriteOnly | QIODevice::Truncate)){
		qDebug() << "open doc/textdisplay failed";
		exit(2);
	}else{
		QTextStream out(&textfile);	
		out << str;	
	}
	textfile.close();
}
void TextPlace::setcolor(int r, int g, int b, int a)		//rgba方式更改颜色
{
	QPalette plt;

	plt = text->palette();
	text_color.setRgb(r, g, b, a);
	plt.setColor(QPalette::WindowText, text_color);
	text->setPalette(plt);
}

void TextPlace::setcolor(int c, int m, int y, int k, int a) //cmyk方式更改颜色
{
	QPalette plt;

	plt = text->palette();
	text_color.setCmyk(c, m, y, k, a);
	plt.setColor(QPalette::WindowText, text_color);
	text->setPalette(plt);
}

void TextPlace::setfontsize(int size)	//设置字号
{
	text_size = size;		//更新字号 成员变量
	text->setFont(QFont(QString::fromUtf8(text_family), text_size));	//字体字号
	textwidth = text->fontMetrics().width(text->text()) * 1;	//更新全部字符宽度, 字号不同, 整个字符宽度也会不同
	//qDebug() << "字体:" << text->fontInfo().family();
}

void TextPlace::setfontfamily(char *fontfamily)		//设置字体
{
	text_family = fontfamily;	//更新字体名字
	text->setFont(QFont(QString::fromUtf8(fontfamily), text_size));		//字体字号
	textwidth = text->fontMetrics().width(text->text()) * 1;	//更新全部字符宽度, 字体不同, 整个字符宽度可能不同
}

void TextPlace::dynamic_displaytext()			//动态显示字幕
{
	i -= text->fontMetrics().maxWidth() / smoothness;//最大字符的宽度除了平滑度, 得到每次显示字符的比例, 除1表示一次显示一个字符

	if((i + textwidth) <= 0)
		i = width();
	text->setGeometry(i, height() * 6.3, textwidth, height() * 0.8);//每次更改显示位置, 看起来就像滚动效果
}

void TextPlace::change_display_mode()			//切换静态/动态显示模式
{
	if (isdynamic) {		//当前状态是动态
		disconnect(texttimer, SIGNAL(timeout()), this, SLOT(dynamic_displaytext()));	//取消槽连接
		text->setGeometry(0, height() * 6.3, textwidth, height() * 0.8);	//始终显示最前面的内容, 内容太多则后面窗口中看不到
	}
	else {					//当前状态是静态
		i = width();		//更新i值, 让每次切换到动态时从头开始
		connect(texttimer, SIGNAL(timeout()), this, SLOT(dynamic_displaytext()));		//建立槽连接
	}
	isdynamic = !isdynamic;	//改变标志位
}

void TextPlace::change_show_hide()				//显示/隐藏
{
	if (isshow)
		text->hide();
	else
		text->show();
	isshow = !isshow;
}

void TextPlace::resizeEvent(QResizeEvent *event)
{
	text_size = height() * 0.4;						//窗口缩放时, 让字号发生更改
	text->setFont(QFont(QString::fromUtf8(text_family), text_size)); //重新设置字体字号
	text->setGeometry(i, height()*6.3, width(), height()*0.8);	//更新显示位置
	textwidth = text->fontMetrics().width(text->text()) * 1;		//更新字总宽度, 不然会出现当总字少于一屏时显示不完全情况
}
