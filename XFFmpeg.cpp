#include "XFFmpeg.h"
#pragma comment(lib,"avformat.lib")		//文件格式
#pragma comment(lib,"avutil.lib")		//工具类
#pragma comment(lib,"avcodec.lib")		//编码库
#pragma comment(lib,"swscale.lib")		//视频转码
#pragma comment(lib,"swresample.lib")	//音频重采样

//基于time base 除法，将pts转换为s,还能获取FPS
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
	totalMs = (ic->duration / AV_TIME_BASE)*1000;				//计算视频总长，得到ms

	for (int i = 0; i < ic->nb_streams; ++i){
		AVCodecContext *enc = ic->streams[i]->codec;						//AVCodecContext 解码器的值
		if (enc->codec_type == AVMEDIA_TYPE_VIDEO){							//如果是视频流，记录其下标
			videoStream = i;
			fps = r2d(ic->streams[i]->avg_frame_rate);						//获取fps
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);			//查找解码器
			if (!codec){
				mutex.unlock();
				printf("video code not find!\n");
				return 0;
			}
			int err = avcodec_open2(enc, codec, NULL);						//打开解码器
			if (err != 0){
				mutex.unlock();
				av_strerror(re, errbuff, sizeof(errbuff));
				printf(errbuff);
				return 0;
			}
		}
		else if (enc->codec_type == AVMEDIA_TYPE_AUDIO){				//音频流操作
			printf("succeed get audio!!!\n");
			audioStream = i;
			AVCodec *codec = avcodec_find_decoder(enc->codec_id);
			if (!codec){
				mutex.unlock();
				printf("audio code not find!\n");
				return 0;
			}
			int err = avcodec_open2(enc, codec, NULL);						//打开解码器
			if (err != 0){
				mutex.unlock();
				av_strerror(re, errbuff, sizeof(errbuff));
				printf(errbuff);
				return 0;
			}
			this->sampleRate = enc->sample_rate;
			this->channel = enc->channels;
			switch (enc->sample_fmt)				//可扩展
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
	if (ic) avformat_close_input(&ic);			//释放ic
	if (yuv) av_frame_free(&yuv);				//释放yuv
	if (cCtx){									//释放cCtx
		sws_freeContext(cCtx);
		cCtx = NULL;
	}
	if (aCtx){									//释放aCtx
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
	int err = av_read_frame(ic, &pkt);			//读取视频帧
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
		yuv = av_frame_alloc();						//第一次需要申请该yuv对象的空间
	}
	if (pcm == NULL){
		pcm = av_frame_alloc();						
	}
	AVFrame* frame = yuv;							//音频帧视频帧分开处理。
	if (pkt->stream_index == audioStream){
		frame = pcm;
	}
	//找到解码器，视频音频对应不同的解码器，但此时还没有打开
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
	int p = (frame->pts * r2d(ic->streams[pkt->stream_index]->time_base)) * 1000;	//计算当前时间 妈的 忘记*1000了
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
	stamp = pos * ic->streams[videoStream]->duration;			//跳转的位置
	int re = av_seek_frame(ic, videoStream, stamp, AVSEEK_FLAG_BACKWARD | AVSEEK_FLAG_FRAME);
	//AVSEEK_FLAG_BACKWARD 表示找的位置时间戳不存在，往前找， AVSEEK_FLAG_FRAME表示只取关键帧不取P帧B帧
	avcodec_flush_buffers(ic->streams[videoStream]->codec);		//将已缓存的解码帧清空
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
	AVCodecContext *ctx = ic->streams[this->audioStream]->codec;			//设置采样格式
	if (aCtx == NULL){
		aCtx = swr_alloc();
		swr_alloc_set_opts(aCtx, ctx->channel_layout, AV_SAMPLE_FMT_S16,	//输出格式
			ctx->sample_rate,
			ctx->channels,
			ctx->sample_fmt,
			ctx->sample_rate,
			0,0);
		swr_init(aCtx);
	}
	uint8_t *data[1];
	data[0] = (uint8_t*)out;
	int len = swr_convert(aCtx, data, 10000, (const uint8_t**)pcm->data, pcm->nb_samples);		//进行重采样
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
	cCtx = sws_getCachedContext(cCtx,						//原解码器
		videoCtx->width,									//源：宽			
		videoCtx->height,									//源：高
		videoCtx->pix_fmt,									//源：像素点的格式
		outwidth,											//输出：宽
		outheight,											//输出：高
		AV_PIX_FMT_BGRA,									//输出像素点格式，适应QT，A表示透明通道
		SWS_BICUBIC,										//转码算法
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
	linesize[0] = outwidth * 4;								//一行的宽度，这么多个像素，一个像素对应 4 字节
	int h = sws_scale(cCtx,									//转码器,函数返回转码后的高度
					yuv->data,									//原数据
					yuv->linesize,
					0,
					videoCtx->height,
					data,										//转码数据
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
