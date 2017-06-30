#include "XAudioPlay.h"
#include <QAudioOutput>
#include <QMutex>
class CXAudioPlay :public XAudioPlay{
public:
	QMutex mutex;
	QAudioOutput* output = NULL;
	QIODevice* io = NULL;
	void Stop(){								//控制新的音频打开，清楚之前的。
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
		QAudioFormat fmt;						//指定音频输出的格式
		fmt.setSampleRate(this->sampleRate);	//1s钟采集了48000个音频
		fmt.setSampleSize(this->sampleSize);	//每个音频的声音大小不同 16位 2字节
		fmt.setChannelCount(this->channel);		//采用双声道
		fmt.setCodec("audio/pcm");				//设定格式 pcm普通格式，未压缩的
		fmt.setByteOrder(QAudioFormat::LittleEndian);
		fmt.setSampleType(QAudioFormat::UnSignedInt);	//每个音频存放的格式
		output = new QAudioOutput(fmt);			//QT操作对象
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
		int free = output->bytesFree();				//通过out获取缓存剩余空间
		mutex.unlock();
		return free;
	}
	bool Write(const char* data, int datasize){
		//从缓冲区中读音频交给QT播放，需要判断缓冲区是否满
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
	static CXAudioPlay ap;		//单例模式 子类的实例
	return &ap;
}
