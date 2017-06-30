#include "videoWidget.h"
#include <qpainter>
#include "XFFmpeg.h"
#include "XVideoThread.h"

void videoWidget::paintEvent(QPaintEvent *e){
	static QImage *image = NULL;
	static int w = 0;		//记录宽
	static int h = 0;		//记录高
	if (w != width() || h != height()){//如果记录的宽高与当前宽高不同
		if (image){						//第一次进来不删除
			delete image->bits();		//删除image内部空间
			delete image;
			image = NULL;
		}
	}
	if (image == NULL){
		uchar *buf = new uchar[width() * height() * 4];	//用于存放解码转码后的空间。
		image = new QImage(buf, width(), height(), QImage::Format_ARGB32);	//四个字节 三个颜色 透明通道
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
	painter.begin(this);							//this指这个窗口
	painter.drawImage(QPoint(0, 0), *image);			//QPoint 位置，image绘制的图片。
	painter.end();
}
void videoWidget::timerEvent(QTimerEvent *e){
	this->update();
}

videoWidget::videoWidget(QWidget *p) : QOpenGLWidget(p)
{
	//XFFmpeg::Get()->Open("video.mkv");
	startTimer(20);
	XVideoThread::Get()->start();		//启动线程
}


videoWidget::~videoWidget()
{
}
