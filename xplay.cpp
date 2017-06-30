#include "xplay.h"
#include "XFFmpeg.h"
#include <QFileDialog.h>				//能够打开界面让用户自己选择
#include <QMessageBox>
#include <XAudioPlay.h>
#define PAUSE "QPushButton{border-image:url(:/Xplay/pausebefore.png);}"
#define PLAY "QPushButton{border-image:url(:/Xplay/playbefore.png);}"

static bool isPressSlider = false;
static bool isPlay = true;
Xplay::Xplay(QWidget *parent)
	: QWidget(parent)
{
	ui.setupUi(this);
	startTimer(40);					//1s刷25次。
	//openFile("video.mkv");
}

Xplay::~Xplay()
{
	
}

void Xplay::resizeEvent(QResizeEvent *e){
	ui.openGLWidget->resize(size());
	ui.playButton->move(100, this->height() - 80);
	ui.openButton->move(50, this->height() - 80);
	ui.playslider->move(25, this->height() - 120);
	ui.playslider->resize(this->width() - 25, ui.playslider->height());
	ui.totaltime->move(this->width()-70, this->height() - 80);
	ui.fenge->move(this->width()-80, this->height() - 80);
	ui.playtime->move(this->width()-130, this->height() - 80);
}

void Xplay::timerEvent(QTimerEvent *e){							//当前时间进度 定时器刷新
	int min = (XFFmpeg::Get()->pts / 1000) / 60;
	int sec = (XFFmpeg::Get()->pts / 1000) % 60;
	char buf[1024] = { 0 };
	sprintf(buf, "%03d:%02d", min, sec);
	ui.playtime->setText(buf);

	if (XFFmpeg::Get()->totalMs > 0){
		float rate = (float)XFFmpeg::Get()->pts / (float)XFFmpeg::Get()->totalMs;	//用于进度条显示
		if (!isPressSlider)									//	按下按钮就不刷新进度条了
			ui.playslider->setValue(rate * 1000);
	}

}

void Xplay::play(){
	isPlay = !isPlay;
	XFFmpeg::Get()->isPlay = isPlay;
	if (isPlay)
		ui.playButton->setStyleSheet(PAUSE);
	else 
		ui.playButton->setStyleSheet(PLAY);
}

void Xplay::openFile(QString name){
	if (name.isEmpty())
		return;
	this->setWindowTitle(name);							//设置播放器顶部字
	int totalMs = XFFmpeg::Get()->Open(name.toLocal8Bit());
	if (totalMs <= 0){		//将QString name转换为char*
		QMessageBox::information(this, "err", "open failed");	//打开失败弹出窗口显示失败
		return;
	}

	XAudioPlay::Get()->sampleRate = XFFmpeg::Get()->sampleRate;	//获取参数
	XAudioPlay::Get()->sampleSize = 16;			//直接重采样为16
	XAudioPlay::Get()->channel = XFFmpeg::Get()->channel;
	printf("rate size channel %d %d %d\n", XAudioPlay::Get()->sampleRate, XAudioPlay::Get()->sampleSize, XAudioPlay::Get()->channel);
	XAudioPlay::Get()->Start();				//启动音频

	char buf[1024] = { 0 };										//总时间显示
	int min = (totalMs / 1000) / 60;
	int sec = (totalMs / 1000) % 60;
	sprintf(buf, "%03d:%02d", min, sec);
	ui.totaltime->setText(buf);
	//Xplay是主窗口类，所有成员存放在ui 对象中。
	//使得打开播放视频时，播放按钮变成暂停按钮
	isPlay = false;
	play();
}

void Xplay::open(){
	//获取打开文件的文件名
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("选择视频文件"));
	openFile(name);
}

//用于进度条的拖动。
void  Xplay::sliderPress(){
	isPressSlider = true;
}
void  Xplay::sliderRelease(){
	isPressSlider = false;
	float pos = 0;
	pos = (float)ui.playslider->value() / (float)(ui.playslider->maximum() + 1);
	XFFmpeg::Get()->Seek(pos);
}