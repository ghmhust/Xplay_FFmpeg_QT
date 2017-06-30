#ifndef XPLAY_H
#define XPLAY_H

#include <QtWidgets/QWidget>
#include "ui_xplay.h"

class Xplay : public QWidget
{
	Q_OBJECT

public:
	Xplay(QWidget *parent = 0);
	~Xplay();
	void resizeEvent(QResizeEvent *e); //重构窗口大小发生变化的事件
	void timerEvent(QTimerEvent *e); //定时器用于定时刷新进度条
	void openFile(QString name);
public slots:						//定义了一个open槽
	void open();
	void sliderPress();
	void sliderRelease();
	void play();
	
private:
	Ui::XplayClass ui;
};

#endif // XPLAY_H
