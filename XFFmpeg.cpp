#include "XFFmpeg.h"
#pragma comment(lib,"avformat.lib")		//�ļ���ʽ
#pragma comment(lib,"avutil.lib")		//������
#pragma comment(lib,"avcodec.lib")		//�����
#pragma comment(lib,"swscale.lib")		//��Ƶת��
#pragma comment(lib,"swresample.lib")	//��Ƶ�ز���

//����time base ��������ptsת��Ϊs,���ܻ�ȡFPS
static double r2d(AVRational r){
	return r.num == 0 || r.den == 0 ? 0. : double(r.num) / double(r.den);
}

int XFFmpeg::Open(const char* path){
	Close();
	mutex.lock();
	int re = avformat_open_input(&ic, path, 0, 0);
	if (re != 0){
		mutex.unlock();
		av_strerror(re, errbuff, sizeof(errbuff));
		printf("open %s failed:%s\n", path, errbuff);
		return 0;
	}
	totalMs = (ic->duration / AV_TIME_BASE)*1000;				//������Ƶ�ܳ����õ�ms

	for (int i = 0; i < ic->nb_streams; ++i){
		AVCodecContext *enc = ic->streams[i]->codec;						//AVCodecContext ��������ֵ
		if (enc->codec_type == AVMEDIA_TYPE_VIDEO){							//�������Ƶ������¼���±�
			videoStream = i;
			fps = r2d(ic->streams[i]->avg_frame_rate);						//��ȡfps
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);			//���ҽ�����
			if (!codec){
				mutex.unlock();
				printf("video code not find!\n");
				return 0;
			}
			int err = avcodec_open2(enc, codec, NULL);						//�򿪽�����
			if (err != 0){
				mutex.unlock();
				av_strerror(re, errbuff, sizeof(errbuff));
				printf(errbuff);
				return 0;
			}
		}
		else if (enc->codec_type == AVMEDIA_TYPE_AUDIO){				//��Ƶ������
			printf("succeed get audio!!!\n");
			audioStream = i;
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);
			if (!codec){
				mutex.unlock();
				printf("audio code not find!\n");
				return 0;
			}
			int err = avcodec_open2(enc, codec, NULL);						//�򿪽�����
			if (err != 0){
				mutex.unlock();
				av_strerror(re, errbuff, sizeof(errbuff));
				printf(errbuff);
				return 0;
			}
			this->sampleRate = enc->sample_rate;
			this->channel = enc->channels;
			switch (enc->sample_fmt)				//����չ
			{
			case AV_SAMPLE_FMT_S16:
				this->sampleSize = 16;
				break;
			case AV_SAMPLE_FMT_S32:
				this->sampleSize = 32;
			default:
				break;
			}
			printf("sample succeed! %d %d %d \n", this->sampleRate, this->sampleSize, this->channel);
		}
	}
	mutex.unlock();
	return totalMs;
}
void XFFmpeg::Close(){
	mutex.lock();
	if (ic) avformat_close_input(&ic);			//�ͷ�ic
	if (yuv) av_frame_free(&yuv);				//�ͷ�yuv
	if (cCtx){									//�ͷ�cCtx
		sws_freeContext(cCtx);
		cCtx = NULL;
	}
	if (aCtx){									//�ͷ�aCtx
		swr_free(&aCtx);
	}
	mutex.unlock();
}
std::string XFFmpeg::GetError(){
	mutex.lock();
	std::string re = this->errbuff;
	mutex.unlock();
	return re;
}

AVPacket XFFmpeg::Read(){
	AVPacket pkt;
	memset(&pkt, 0, sizeof(pkt));
	mutex.lock();
	if (!ic){
		mutex.unlock();
		return pkt;
	}
	int err = av_read_frame(ic, &pkt);			//��ȡ��Ƶ֡
	if (err != 0){
		av_strerror(err, errbuff, sizeof(errbuff));
	}
	mutex.unlock();
	return pkt;
}

int XFFmpeg::Decode(const AVPacket *pkt){
	mutex.lock();
	if (!ic){
		mutex.unlock();
		return NULL;
	}
	if (yuv == NULL){
		yuv = av_frame_alloc();						//��һ����Ҫ�����yuv����Ŀռ�
	}
	if (pcm == NULL){
		pcm = av_frame_alloc();						
	}
	AVFrame* frame = yuv;							//��Ƶ֡��Ƶ֡�ֿ�����
	if (pkt->stream_index == audioStream){
		frame = pcm;
	}
	//�ҵ�����������Ƶ��Ƶ��Ӧ��ͬ�Ľ�����������ʱ��û�д�
	int re = avcodec_send_packet(ic->streams[pkt->stream_index]->codec, pkt);
	if (re != 0){
		mutex.unlock();
		return NULL;
	}
	re = avcodec_receive_frame(ic->streams[pkt->stream_index]->codec, frame);
	if (re != 0){
		mutex.unlock();
		return NULL;
	}
	int p = (frame->pts * r2d(ic->streams[pkt->stream_index]->time_base)) * 1000;	//���㵱ǰʱ�� ��� ����*1000��
	if (pkt->stream_index == audioStream) this->pts = p;
	mutex.unlock();
	return p;
}

bool XFFmpeg::Seek(float pos){
	mutex.lock();
	if (!ic){
		mutex.unlock();
		return false;
	}
	int64_t stamp = 0;
	stamp = pos * ic->streams[videoStream]->duration;			//��ת��λ��
	int re = av_seek_frame(ic, videoStream, stamp, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	//AVSEEK_FLAG_BACKWARD ��ʾ�ҵ�λ��ʱ��������ڣ���ǰ�ң� AVSEEK_FLAG_FRAME��ʾֻȡ�ؼ�֡��ȡP֡B֡
	avcodec_flush_buffers(ic->streams[videoStream]->codec);		//���ѻ���Ľ���֡���
	pts = stamp * r2d(ic->streams[videoStream]->time_base) * 1000;
	if (re >= 0){
		mutex.unlock();
		return true;
	}
	mutex.unlock();
	return false;
}

int XFFmpeg::ToPCM(char* out){
	mutex.lock();
	if (!ic || !out || !pcm){
		mutex.unlock();
		return 0;
	}
	AVCodecContext *ctx = ic->streams[this->audioStream]->codec;			//���ò�����ʽ
	if (aCtx == NULL){
		aCtx = swr_alloc();
		swr_alloc_set_opts(aCtx, ctx->channel_layout, AV_SAMPLE_FMT_S16,	//�����ʽ
			ctx->sample_rate,
			ctx->channels,
			ctx->sample_fmt,
			ctx->sample_rate,
			0,0);
		swr_init(aCtx);
	}
	uint8_t *data[1];
	data[0] = (uint8_t*)out;
	int len = swr_convert(aCtx, data, 10000, (const uint8_t**)pcm->data, pcm->nb_samples);		//�����ز���
	if (len <= 0){
		mutex.unlock();
		return 0;
	}
	int outsize = av_samples_get_buffer_size(NULL, ctx->channels, pcm->nb_samples, AV_SAMPLE_FMT_S16, 0);
	if (outsize > 0){
		printf("pcm transform succeed {%d}\n", outsize);
	}
	mutex.unlock();
	return outsize;
}

bool XFFmpeg::ToRGB(char* out, int outwidth, int outheight){
	mutex.lock();
	if (!ic || !yuv){
		mutex.unlock();
		return false;
	}
	AVCodecContext *videoCtx = ic->streams[this->videoStream]->codec;
	cCtx = sws_getCachedContext(cCtx,						//ԭ������
		videoCtx->width,									//Դ����			
		videoCtx->height,									//Դ����
		videoCtx->pix_fmt,									//Դ�����ص�ĸ�ʽ
		outwidth,											//�������
		outheight,											//�������
		AV_PIX_FMT_BGRA,									//������ص��ʽ����ӦQT��A��ʾ͸��ͨ��
		SWS_BICUBIC,										//ת���㷨
		NULL, NULL, NULL
		);
	if (!cCtx){
		mutex.unlock();
		printf("sws_getCachedContext failed\n");
		return false;
	}
	uint8_t *data[AV_NUM_DATA_POINTERS] = { 0 };
	data[0] = (uint8_t*)out;
	int linesize[AV_NUM_DATA_POINTERS] = { 0 };
	linesize[0] = outwidth * 4;								//һ�еĿ�ȣ���ô������أ�һ�����ض�Ӧ 4 �ֽ�
	int h = sws_scale(cCtx,									//ת����,��������ת���ĸ߶�
					yuv->data,									//ԭ����
					yuv->linesize,
					0,
					videoCtx->height,
					data,										//ת������
					linesize
	);
	/*if (h > 0){
		printf("rgb transform succeed {%d}\n", h);
	}*/
	mutex.unlock();
	return true;
}

XFFmpeg::XFFmpeg()
{
	errbuff[0] = '\0';
	av_register_all();
}


XFFmpeg::~XFFmpeg()
{
}
