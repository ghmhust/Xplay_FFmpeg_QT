#pragma once
extern "C"{
#include <libavformat\avformat.h>
#include <libswscale\swscale.h>
#include <libswresample\swresample.h>	//用于音频的重采样
}
#include <string>
#include <QMutex>						//QT自带的互斥变量
class XFFmpeg
{
public:
	static XFFmpeg *Get(){				//单例模式，播放视频只需要解码一个，不用用户去维护。
		static XFFmpeg ff;
		return &ff;
	}
	/*
	@para path 文件路径
	@return int 返回视频的总时长 成功失败
	*/
	int Open(const char* path);		//打开视频,如果视频已打开，需要先关闭再打开
	void Close();
	std::string GetError();				//获取错误信息，返回string而不是返回char*
	/*
	读取视频帧，返回数据用户处理。解码，转码
	*/
	AVPacket Read();
	//返回pts，只有音频才会改变这个类对象的pts成员
	int Decode(const AVPacket *pkt);

	int ToPCM(char* out);
	bool ToRGB(char* out, int outwidth, int outheight);

	bool Seek(float pos);			//传一个百分比位置用于进度条控制

	int totalMs = 0;
	int fps = 0;
	int pts = 0;						//表示当前进度
	int videoStream = 0;
	int audioStream = 1;
	int sampleRate = 48000;
	int sampleSize = 16;
	int channel = 2;
	bool isPlay = false;
	virtual ~XFFmpeg();
protected:								//构造函数放在保护类型，使得外部不能创建对象
	char errbuff[1024];
	AVFormatContext *ic = NULL;
	AVFrame *yuv = NULL;
	AVFrame *pcm = NULL;				//为音频开辟一块内存
	SwsContext *cCtx = NULL;
	SwrContext *aCtx = NULL;
	QMutex mutex;
	XFFmpeg();
};

