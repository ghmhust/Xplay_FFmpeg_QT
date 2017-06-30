#include "xplay.h"
#include "XFFmpeg.h"
#include <QFileDialog.h>				//�ܹ��򿪽������û��Լ�ѡ��
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
	startTimer(40);					//1sˢ25�Ρ�
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

void Xplay::timerEvent(QTimerEvent *e){							//��ǰʱ����� ��ʱ��ˢ��
	int min = (XFFmpeg::Get()->pts / 1000) / 60;
	int sec = (XFFmpeg::Get()->pts / 1000) % 60;
	char buf[1024] = { 0 };
	sprintf(buf, "%03d:%02d", min, sec);
	ui.playtime->setText(buf);

	if (XFFmpeg::Get()->totalMs > 0){
		float rate = (float)XFFmpeg::Get()->pts / (float)XFFmpeg::Get()->totalMs;	//���ڽ�������ʾ
		if (!isPressSlider)									//	���°�ť�Ͳ�ˢ�½�������
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
	this->setWindowTitle(name);							//���ò�����������
	int totalMs = XFFmpeg::Get()->Open(name.toLocal8Bit());
	if (totalMs <= 0){		//��QString nameת��Ϊchar*
		QMessageBox::information(this, "err", "open failed");	//��ʧ�ܵ���������ʾʧ��
		return;
	}

	XAudioPlay::Get()->sampleRate = XFFmpeg::Get()->sampleRate;	//��ȡ����
	XAudioPlay::Get()->sampleSize = 16;			//ֱ���ز���Ϊ16
	XAudioPlay::Get()->channel = XFFmpeg::Get()->channel;
	printf("rate size channel %d %d %d\n", XAudioPlay::Get()->sampleRate, XAudioPlay::Get()->sampleSize, XAudioPlay::Get()->channel);
	XAudioPlay::Get()->Start();				//������Ƶ

	char buf[1024] = { 0 };										//��ʱ����ʾ
	int min = (totalMs / 1000) / 60;
	int sec = (totalMs / 1000) % 60;
	sprintf(buf, "%03d:%02d", min, sec);
	ui.totaltime->setText(buf);
	//Xplay���������࣬���г�Ա�����ui �����С�
	//ʹ�ô򿪲�����Ƶʱ�����Ű�ť�����ͣ��ť
	isPlay = false;
	play();
}

void Xplay::open(){
	//��ȡ���ļ����ļ���
	QString name = QFileDialog::getOpenFileName(this, QString::fromLocal8Bit("ѡ����Ƶ�ļ�"));
	openFile(name);
}

//���ڽ��������϶���
void  Xplay::sliderPress(){
	isPressSlider = true;
}
void  Xplay::sliderRelease(){
	isPressSlider = false;
	float pos = 0;
	pos = (float)ui.playslider->value() / (float)(ui.playslider->maximum() + 1);
	XFFmpeg::Get()->Seek(pos);
}