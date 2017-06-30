#include "XAudioPlay.h"
#include <QAudioOutput>
#include <QMutex>
class CXAudioPlay :public XAudioPlay{
public:
	QMutex mutex;
	QAudioOutput* output = NULL;
	QIODevice* io = NULL;
	void Stop(){								//�����µ���Ƶ�򿪣����֮ǰ�ġ�
		mutex.lock();
		if (output){
			output->stop();
			delete output;
			output = NULL;
			io = NULL;
		}
		mutex.unlock();
	}
	bool Start(){
		Stop();
		mutex.lock();
		QAudioFormat fmt;						//ָ����Ƶ����ĸ�ʽ
		fmt.setSampleRate(this->sampleRate);	//1s�Ӳɼ���48000����Ƶ
		fmt.setSampleSize(this->sampleSize);	//ÿ����Ƶ��������С��ͬ 16λ 2�ֽ�
		fmt.setChannelCount(this->channel);		//����˫����
		fmt.setCodec("audio/pcm");				//�趨��ʽ pcm��ͨ��ʽ��δѹ����
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);	//ÿ����Ƶ��ŵĸ�ʽ
		output = new QAudioOutput(fmt);			//QT��������
		io = output->start();						//
		mutex.unlock();
		return true;
	}
	void Play(bool isplay){
		mutex.lock();
		if (!output){
			mutex.unlock();
			return;
		}
		if (isplay)	output->resume();
		else output->suspend();
		mutex.unlock();
	}
	int GetFree(){
		mutex.lock();
		if (!output){
			mutex.unlock();
			return 0;
		}
		int free = output->bytesFree();				//ͨ��out��ȡ����ʣ��ռ�
		mutex.unlock();
		return free;
	}
	bool Write(const char* data, int datasize){
		//�ӻ������ж���Ƶ����QT���ţ���Ҫ�жϻ������Ƿ���
		mutex.lock();
		if (!data || datasize <= 0){
			mutex.unlock();
			return false;
		}
		if (io)
			io->write(data, datasize);
		else{
			mutex.unlock();
			return false;
		}
		mutex.unlock();
		return true;
	}
};

XAudioPlay::XAudioPlay()
{
}


XAudioPlay::~XAudioPlay()
{
}


XAudioPlay* XAudioPlay::Get(){
	static CXAudioPlay ap;		//����ģʽ �����ʵ��
	return &ap;
}
