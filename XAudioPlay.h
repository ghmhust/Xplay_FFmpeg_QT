#pragma once
class XAudioPlay				//定义纯虚函数，就变成了一个接口类
{
public:
	static XAudioPlay *Get();
	virtual bool Start() = 0;						//开始播放
	virtual void Stop() = 0;						//重新打开视频音频清0
	virtual void Play(bool isplay) = 0;				//音频播放暂停控制
	virtual bool Write(const char* data, int datasize) = 0;	//写入音频数据
	virtual int GetFree() = 0;						//获取缓存剩余空间
	virtual ~XAudioPlay();
	int sampleRate = 48000;
	int sampleSize = 16;
	int channel = 2;
protected:
	XAudioPlay();		//肯定只有一个音频播放类，内部操作
};

