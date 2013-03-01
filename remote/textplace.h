#ifndef TEXTPLACE_H
#define TEXTPLACE_H

#include <QLabel>
#include <QTimer>
#define TEXT_DISPLAY "doc/textdisplay"
class TextPlace : public QLabel
{
	Q_OBJECT
public:
	TextPlace(QWidget *parent = 0);	
	QLabel *text;			
	QColor text_color;			
	QByteArray text_family;	
	int text_size;			
	QTimer *texttimer;			
	float smoothness;			


	void setmsg();	
	void change_content(QString str);
	void setfontsize(int);		
	void setfontfamily(char *);	
	void setcolor(int r, int g, int b, int a);			//设置文字颜色(RGB方式)
	void setcolor(int c, int m, int y, int k, int a);	//设置文字颜色(CMYK方式)
	void change_display_mode(void);		//改变显示模式, 静态/动态
	void change_show_hide(void);		//显示与隐藏
public slots:
	void dynamic_displaytext(void);		//动态显示处理函数, 槽函数, 时钟超时触发
	void resizeEvent(QResizeEvent *event);

private:
	int i;				//动态显示, 循环的变量, 记录要显示的x坐标
	int textwidth;		//字符占用总宽度
	bool isdynamic;		//动态显示标志			true动态, false静态
	bool isshow;		//是否显示/隐藏标志	true显示, false隐藏


};

#endif // TEXTAREA_H
