#pragma once
#include <QtWidgets/qwidget.h>
#include <QOpenGLWidget>				//�̳�Qwidget

class videoWidget:public QOpenGLWidget
{
public:
	videoWidget(QWidget *p = NULL);						//QT�����У����캯���̳и���
	void paintEvent(QPaintEvent *e);					//�ػ溯��
	void timerEvent(QTimerEvent *e);					//QT��ʱ����ˢ���ػ�
	virtual ~videoWidget();
};

