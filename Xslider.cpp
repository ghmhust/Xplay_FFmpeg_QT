#include "Xslider.h"
#include <QMouseEvent>

void Xslider::mousePressEvent(QMouseEvent *e){
	int value = ((float)e->pos().x() / (float)this->width())*(this->maximum() + 1);
	this->setValue(value);				//value值在xplay的seek函数调用前用到
	QSlider::mousePressEvent(e);
}
Xslider::Xslider(QWidget *p) :QSlider(p)		//传参传给基类的构造函数
{
}


Xslider::~Xslider()
{
}
