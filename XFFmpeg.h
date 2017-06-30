#pragma once
extern "C"{
#include <libavformat\avformat.h>
#include <libswscale\swscale.h>
#include <libswresample\swresample.h>	//������Ƶ���ز���
}
#include <string>
#include <QMutex>						//QT�Դ��Ļ������
class XFFmpeg
{
public:
	static XFFmpeg *Get(){				//����ģʽ��������Ƶֻ��Ҫ����һ���������û�ȥά����
		static XFFmpeg ff;
		return &ff;
	}
	/*
	@para path �ļ�·��
	@return int ������Ƶ����ʱ�� �ɹ�ʧ��
	*/
	int Open(const char* path);		//����Ƶ,�����Ƶ�Ѵ򿪣���Ҫ�ȹر��ٴ�
	void Close();
	std::string GetError();				//��ȡ������Ϣ������string�����Ƿ���char*
	/*
	��ȡ��Ƶ֡�����������û��������룬ת��
	*/
	AVPacket Read();
	//����pts��ֻ����Ƶ�Ż�ı����������pts��Ա
	int Decode(const AVPacket *pkt);

	int ToPCM(char* out);
	bool ToRGB(char* out, int outwidth, int outheight);

	bool Seek(float pos);			//��һ���ٷֱ�λ�����ڽ���������

	int totalMs = 0;
	int fps = 0;
	int pts = 0;						//��ʾ��ǰ����
	int videoStream = 0;
	int audioStream = 1;
	int sampleRate = 48000;
	int sampleSize = 16;
	int channel = 2;
	bool isPlay = false;
	virtual ~XFFmpeg();
protected:								//���캯�����ڱ������ͣ�ʹ���ⲿ���ܴ�������
	char errbuff[1024];
	AVFormatContext *ic = NULL;
	AVFrame *yuv = NULL;
	AVFrame *pcm = NULL;				//Ϊ��Ƶ����һ���ڴ�
	SwsContext *cCtx = NULL;
	SwrContext *aCtx = NULL;
	QMutex mutex;
	XFFmpeg();
};

