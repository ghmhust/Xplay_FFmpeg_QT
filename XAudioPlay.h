#pragma once
class XAudioPlay				//���崿�麯�����ͱ����һ���ӿ���
{
public:
	static XAudioPlay *Get();
	virtual bool Start() = 0;						//��ʼ����
	virtual void Stop() = 0;						//���´���Ƶ��Ƶ��0
	virtual void Play(bool isplay) = 0;				//��Ƶ������ͣ����
	virtual bool Write(const char* data, int datasize) = 0;	//д����Ƶ����
	virtual int GetFree() = 0;						//��ȡ����ʣ��ռ�
	virtual ~XAudioPlay();
	int sampleRate = 48000;
	int sampleSize = 16;
	int channel = 2;
protected:
	XAudioPlay();		//�϶�ֻ��һ����Ƶ�����࣬�ڲ�����
};

