#pragma once
#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>				//继承Qwidget

class videoWidget:public QOpenGLWidget
{
public:
	videoWidget(QWidget *p = NULL);						//QT窗口中，构造函数继承父类
	void paintEvent(QPaintEvent *e);					//重绘函数
	void timerEvent(QTimerEvent *e);					//QT定时器，刷新重绘
	virtual ~videoWidget();
};

