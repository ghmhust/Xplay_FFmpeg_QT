#pragma once
#include <qslider.h>
class Xslider:public QSlider
{
	Q_OBJECT		//QT���������,����QT��
public:
	Xslider(QWidget *p = NULL);
	~Xslider();
	void mousePressEvent(QMouseEvent *e);	//���������¼� �ǵ������������Ƶ
};

