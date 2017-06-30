#include "xplay.h"
#include <QtWidgets/QApplication>
#include "XFFmpeg.h"
#include <QAudioOutput>

int main(int argc, char *argv[])
{
	//QAudioOutput *out;		//qt音频库
	//QAudioFormat fmt;		//指定音频输出的格式
	//fmt.setSampleRate(48000);	//1s钟采集了48000个音频
	//fmt.setSampleSize(16);		//每个音频的声音大小不同 16位 2字节
	//fmt.setChannelCount(2);		//采用双声道
	//fmt.setCodec("audio/pcm");	//设定格式 pcm普通格式，未压缩的
	//fmt.setByteOrder(QAudioFormat::LittleEndian);
	//fmt.setSampleType(QAudioFormat::UnSignedInt);	//每个音频存放的格式
	//out = new QAudioOutput(fmt);
	//QIODevice* ad = out->start();			//打开音频 通过ad往里面写音频
	QApplication a(argc, argv);
	Xplay w;
	w.show();
	return a.exec();
}
