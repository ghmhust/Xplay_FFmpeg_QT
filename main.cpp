#include "xplay.h"
#include <QtWidgets/QApplication>
#include "XFFmpeg.h"
#include <QAudioOutput>

int main(int argc, char *argv[])
{
	//QAudioOutput *out;		//qt��Ƶ��
	//QAudioFormat fmt;		//ָ����Ƶ����ĸ�ʽ
	//fmt.setSampleRate(48000);	//1s�Ӳɼ���48000����Ƶ
	//fmt.setSampleSize(16);		//ÿ����Ƶ��������С��ͬ 16λ 2�ֽ�
	//fmt.setChannelCount(2);		//����˫����
	//fmt.setCodec("audio/pcm");	//�趨��ʽ pcm��ͨ��ʽ��δѹ����
	//fmt.setByteOrder(QAudioFormat::LittleEndian);
	//fmt.setSampleType(QAudioFormat::UnSignedInt);	//ÿ����Ƶ��ŵĸ�ʽ
	//out = new QAudioOutput(fmt);
	//QIODevice* ad = out->start();			//����Ƶ ͨ��ad������д��Ƶ
	QApplication a(argc, argv);
	Xplay w;
	w.show();
	return a.exec();
}
