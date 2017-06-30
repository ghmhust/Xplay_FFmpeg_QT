#include "videoWidget.h"
#include <qpainter>
#include "XFFmpeg.h"
#include "XVideoThread.h"

void videoWidget::paintEvent(QPaintEvent *e){
	static QImage *image = NULL;
	static int w = 0;		//��¼��
	static int h = 0;		//��¼��
	if (w != width() || h != height()){//�����¼�Ŀ���뵱ǰ��߲�ͬ
		if (image){						//��һ�ν�����ɾ��
			delete image->bits();		//ɾ��image�ڲ��ռ�
			delete image;
			image = NULL;
		}
	}
	if (image == NULL){
		uchar *buf = new uchar[width() * height() * 4];	//���ڴ�Ž���ת���Ŀռ䡣
		image = new QImage(buf, width(), height(), QImage::Format_ARGB32);	//�ĸ��ֽ� ������ɫ ͸��ͨ��
	}
	/*AVPacket pkt = XFFmpeg::Get()->Read();				
	if (pkt.stream_index != XFFmpeg::Get()->videoStream){
		av_packet_unref(&pkt);
		return;
	}
	if (pkt.size == 0) return;
	AVFrame *yuv = XFFmpeg::Get()->Decode(&pkt);
	av_packet_unref(&pkt);
	if (yuv == NULL) return;*/
	XFFmpeg::Get()->ToRGB((char*)image->bits(), width(), height());
	QPainter painter;
	painter.begin(this);							//thisָ�������
	painter.drawImage(QPoint(0, 0), *image);			//QPoint λ�ã�image���Ƶ�ͼƬ��
	painter.end();
}
void videoWidget::timerEvent(QTimerEvent *e){
	this->update();
}

videoWidget::videoWidget(QWidget *p) : QOpenGLWidget(p)
{
	//XFFmpeg::Get()->Open("video.mkv");
	startTimer(20);
	XVideoThread::Get()->start();		//�����߳�
}


videoWidget::~videoWidget()
{
}
