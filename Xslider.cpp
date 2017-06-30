#include "Xslider.h"
#include <QMouseEvent>

void Xslider::mousePressEvent(QMouseEvent *e){
	int value = ((float)e->pos().x() / (float)this->width())*(this->maximum() + 1);
	this->setValue(value);				//valueֵ��xplay��seek��������ǰ�õ�
	QSlider::mousePressEvent(e);
}
Xslider::Xslider(QWidget *p) :QSlider(p)		//���δ�������Ĺ��캯��
{
}


Xslider::~Xslider()
{
}
