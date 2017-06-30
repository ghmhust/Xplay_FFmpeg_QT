#pragma once
#include <qslider.h>
class Xslider:public QSlider
{
	Q_OBJECT		//QT相关类必须的,重载QT类
public:
	Xslider(QWidget *p = NULL);
	~Xslider();
	void mousePressEvent(QMouseEvent *e);	//添加鼠标点击事件 是的鼠标点击调整视频
};

