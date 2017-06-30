#include "XVideoThread.h"
#include "XFFmpeg.h"
#include "XAudioPlay.h"		//��Ƶ����
using namespace std;
bool isexit = false;		//ȫ�ֱ����������߳��˳�
void XVideoThread::run(){
	char out[10000] = { 0 };
	while (!isexit){
		if (!XFFmpeg::Get()->isPlay){			//��ͣ״̬ ����ȡ��Ƶ
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
			XFFmpeg::Get()->Decode(&pkt);			//��Ƶ���뿪ʼ
			av_packet_unref(&pkt);
			int len = XFFmpeg::Get()->ToPCM(out);	//��Ƶ�ز���
			XAudioPlay::Get()->Write(out, len);		//QT���в���
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
