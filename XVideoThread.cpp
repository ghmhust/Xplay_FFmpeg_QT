#include "XVideoThread.h"
#include "XFFmpeg.h"
#include "XAudioPlay.h"		//音频播放
using namespace std;
bool isexit = false;		//全局变量，负责线程退出
void XVideoThread::run(){
	char out[10000] = { 0 };
	while (!isexit){
		if (!XFFmpeg::Get()->isPlay){			//暂停状态 不读取视频
			msleep(10);
			continue;
		}
		AVPacket pkt = XFFmpeg::Get()->Read();
		if (pkt.size <= 0){
			msleep(10);
			continue;
		}
		int free = XAudioPlay::Get()->GetFree();
		if (free < 10000){
			msleep(10);
			continue;
		}
		if (pkt.stream_index == XFFmpeg::Get()->audioStream){
			XFFmpeg::Get()->Decode(&pkt);			//音频解码开始
			av_packet_unref(&pkt);
			int len = XFFmpeg::Get()->ToPCM(out);	//音频重采样
			XAudioPlay::Get()->Write(out, len);		//QT进行播放
			continue;
		}
		if (pkt.stream_index != XFFmpeg::Get()->videoStream){
			av_packet_unref(&pkt);
			continue;
		}
		XFFmpeg::Get()->Decode(&pkt);
		av_packet_unref(&pkt);
		if (XFFmpeg::Get()->fps > 0){
			msleep(XFFmpeg::Get()->fps);
		}
	}
}
XVideoThread::XVideoThread()
{
}


XVideoThread::~XVideoThread()
{
}
