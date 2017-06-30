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
	void resizeEvent(QResizeEvent *e); //�ع����ڴ�С�����仯���¼�
	void timerEvent(QTimerEvent *e); //��ʱ�����ڶ�ʱˢ�½�����
	void openFile(QString name);
public slots:						//������һ��open��
	void open();
	void sliderPress();
	void sliderRelease();
	void play();
	
private:
	Ui::XplayClass ui;
};

#endif // XPLAY_H
