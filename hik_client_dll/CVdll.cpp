#include "stdafx.h"
#include <stdio.h>  
#include "opencv2/opencv.hpp"
#define __STDC_CONSTANT_MACROS  
using namespace cv;

extern "C"
{
#include "libavformat/avformat.h"  
#include "libavutil/mathematics.h"  
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavutil/time.h"  
};

#include<string.h>  
#include<malloc.h>  

#pragma pack(1)  

int main()
{

	typedef struct tagBITMAPFILEHEADER
	{
		unsigned short  bfType; //2 位图文件的类型，必须为“BM”  
		unsigned long bfSize; //4 位图文件的大小，以字节为单位  
		unsigned short bfReserved1; //2 位图文件保留字，必须为0  
		unsigned short bfReserved2; //2 位图文件保留字，必须为0  
		unsigned long bfOffBits; //4 位图数据的起始位置，以相对于位图文件头的偏移量表示，以字节为单位  
	} BITMAPFILEHEADER;//该结构占据14个字节。  

	typedef struct tagBITMAPINFOHEADER {
		unsigned long biSize; //4 本结构所占用字节数  
		long biWidth; //4 位图的宽度，以像素为单位  
		long biHeight; //4 位图的高度，以像素为单位  
		unsigned short biPlanes; //2 目标设备的平面数不清，必须为1  
		unsigned short biBitCount;//2 每个像素所需的位数，必须是1(双色), 4(16色)，8(256色)或24(真彩色)之一  
		unsigned long biCompression; //4 位图压缩类型，必须是 0(不压缩),1(BI_RLE8压缩类型)或2(BI_RLE4压缩类型)之一  
		unsigned long biSizeImage; //4 位图的大小，以字节为单位  
		long biXPelsPerMeter; //4 位图水平分辨率，每米像素数  
		long biYPelsPerMeter; //4 位图垂直分辨率，每米像素数  
		unsigned long biClrUsed;//4 位图实际使用的颜色表中的颜色数  
		unsigned long biClrImportant;//4 位图显示过程中重要的颜色数  
	} BITMAPINFOHEADER;//该结构占据40个字节。  

	int nWidth = 0;
	int nHeight = 0;

	int nDataLen = 0;

	int nLen;

	char csFileName[20];
	//char csFileName[20];

	int fileI;
	unsigned char *pBmpBuf;
	for (fileI = 1; fileI <= 1; fileI++)
	{
		sprintf(csFileName, "bmp/%d.bmp", fileI);
		printf("%s\n", csFileName);

		FILE *fp;
		if ((fp = fopen(csFileName, "rb")) == NULL)  //以二进制的方式打开文件  
		{
			return FALSE;
		}
		WORD fileType;
		fread(&fileType, 1, sizeof(WORD), fp);
		if (fileType != 0x4d42)
		{
			printf("file is not .bmp file!");
			system("pause");
			return 0;
		}

		if (fseek(fp, sizeof(BITMAPFILEHEADER), 0))  //跳过BITMAPFILEHEADE  
		{
			return FALSE;
		}
		BITMAPINFOHEADER infoHead;
		fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);   //从fp中读取BITMAPINFOHEADER信息到infoHead中,同时fp的指针移动  
		nWidth = infoHead.biWidth;
		nHeight = infoHead.biHeight;
		int linebyte = (nWidth * 24 / 8 + 3) / 4 * 4; //计算每行的字节数，24：该图片是24位的bmp图，3：确保不丢失像素  

													  //cout<<bmpwidth<<" "<<bmpheight<<endl;  
		nDataLen = linebyte*nHeight;
		pBmpBuf = new unsigned char[linebyte*nHeight];
		fread(pBmpBuf, sizeof(char), linebyte*nHeight, fp);
		fclose(fp);
	}
	printf("file ok\n");
	av_register_all();
	avcodec_register_all();
	AVFrame *m_pRGBFrame = new AVFrame[1];  //RGB帧数据      
	AVFrame *m_pYUVFrame = new AVFrame[1];;  //YUV帧数据    
	AVCodecContext *c = NULL;
	AVCodecContext *in_c = NULL;
	AVCodec *pCodecH264; //编码器    
	uint8_t * yuv_buff;//    

					   //查找h264编码器    
	pCodecH264 = avcodec_find_encoder(AV_CODEC_ID_H264);


	c = avcodec_alloc_context3(pCodecH264);
	c->bit_rate = 3000000;// put sample parameters     
	c->width = nWidth;//     
	c->height = nHeight;//     

						// frames per second     
	AVRational rate;
	rate.num = 1;
	rate.den = 5;
	c->time_base = rate;//(AVRational){1,25};    
	c->gop_size = 10; // emit one intra frame every ten frames     
	c->max_b_frames = 1;
	c->thread_count = 1;
	c->pix_fmt = AV_PIX_FMT_YUV420P;//PIX_FMT_RGB24;    

									//av_opt_set(c->priv_data, /*"preset"*/"libvpx-1080p.ffpreset", /*"slow"*/NULL, 0);    
									//打开编码器    
	if (avcodec_open2(c, pCodecH264, NULL)<0)
		printf("不能打开编码库");

	int size = c->width * c->height;

	yuv_buff = (uint8_t *)malloc((size * 3) / 2); // size for YUV 420     

												  //将rgb图像数据填充rgb帧    
	uint8_t * rgb_buff = new uint8_t[nDataLen];

	//图象编码    
	int outbuf_size = 100000;
	uint8_t * outbuf = (uint8_t*)malloc(outbuf_size);
	int u_size = 0;
	/*FILE *h264_f = NULL;
	const char * filename = "0_Data.h264";
	h264_f = fopen(filename, "wb");
	if (!h264_f)
	{
	printf("could not open %s\n", filename);
	exit(1);
	}*/

	//初始化SwsContext    
	SwsContext * scxt = sws_getContext(c->width, c->height, AV_PIX_FMT_BGR24, c->width, c->height, AV_PIX_FMT_YUV420P, SWS_POINT, NULL, NULL, NULL);

	AVPacket avpkt;

	//-----lbg----------
	AVOutputFormat *ofmt = NULL;
	//输入对应一个AVFormatContext，输出对应一个AVFormatContext  
	//（Input AVFormatContext and Output AVFormatContext）  
	AVFormatContext  *ofmt_ctx = NULL;
	//AVPacket pkt;
	const char *in_filename, *out_filename;
	int ret, i;
	int videoindex = -1;
	int frame_index = 0;
	int64_t start_time = 0;

	//in_filename = "F:/FeigeDownload/007.avi";//输入URL（Input file URL）  
	//in_filename  = "shanghai03_p.h264";  

	out_filename = "rtsp://192.168.0.104/test1";//输出 URL（Output URL）[RTMP]  
												//out_filename = "rtp://233.233.233.233:6666";//输出 URL（Output URL）[UDP]  

	av_register_all();
	//Network  
	avformat_network_init();

	//输出（Output）  

	avformat_alloc_output_context2(&ofmt_ctx, NULL, "rtsp", out_filename); //RTMP  
																		   //avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", out_filename);//UDP  

	if (!ofmt_ctx) {
		printf("Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	ofmt = ofmt_ctx->oformat;
	//for (i = 0; i < ifmt_ctx->nb_streams; i++) {
	//根据输入流创建输出流（Create output AVStream according to input AVStream）  
	//AVStream *in_stream = ifmt_ctx->streams[i];
	//AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
	AVStream *out_stream = avformat_new_stream(ofmt_ctx, pCodecH264);
	if (!out_stream) {
		printf("Failed allocating output stream\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	//复制AVCodecContext的设置（Copy the settings of AVCodecContext）  
	/*ret = avcodec_copy_context(out_stream->codec, in_stream->codec);
	if (ret < 0) {
	printf("Failed to copy context from input to output stream codec context\n");
	goto end;
	}*/


	ret = avcodec_copy_context(out_stream->codec, c);
	if (ofmt_ctx->oformat->flags & AVFMT_GLOBALHEADER)
		out_stream->codec->flags |= CODEC_FLAG_GLOBAL_HEADER;
	//}
	//Dump Format------------------  
	av_dump_format(ofmt_ctx, 0, out_filename, 1);
	//打开输出URL（Open output URL）  
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			printf("Could not open output URL '%s'", out_filename);
			goto end;
		}
	}
	//写文件头（Write file header）  

	ret = avformat_write_header(ofmt_ctx, NULL);
	if (ret < 0) {
		printf("Error occurred when opening output URL\n");
		goto end;
	}
	//----------------
	int duation = 0;
	//AVFrame *pTFrame=new AVFrame    
	for (int i = 0; i<250000; ++i)
	{

		//AVFrame *m_pYUVFrame = new AVFrame[1];    

		int index = i % 200 + 1;

		sprintf(csFileName, "bmp/%d.bmp", index);
		printf("%s\n", csFileName);

		FILE *fp;
		if ((fp = fopen(csFileName, "rb")) == NULL)  //以二进制的方式打开文件  
		{
			return FALSE;
		}
		WORD fileType;
		fread(&fileType, 1, sizeof(WORD), fp);
		if (fileType != 0x4d42)
		{
			printf("file is not .bmp file!");
			system("pause");
			return 0;
		}

		if (fseek(fp, sizeof(BITMAPFILEHEADER), 0))  //跳过BITMAPFILEHEADE  
		{
			return FALSE;
		}
		BITMAPINFOHEADER infoHead;
		fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);   //从fp中读取BITMAPINFOHEADER信息到infoHead中,同时fp的指针移动  
		nWidth = infoHead.biWidth;
		nHeight = infoHead.biHeight;
		int linebyte = (nWidth * 24 / 8 + 3) / 4 * 4; //计算每行的字节数，24：该图片是24位的bmp图，3：确保不丢失像素  

													  //cout<<bmpwidth<<" "<<bmpheight<<endl;  
		nDataLen = linebyte*nHeight;
		pBmpBuf = new unsigned char[linebyte*nHeight];
		fread(pBmpBuf, sizeof(char), linebyte*nHeight, fp);
		fclose(fp);
		memcpy(rgb_buff, pBmpBuf, nDataLen);


		avpicture_fill((AVPicture*)m_pRGBFrame, (uint8_t*)rgb_buff, AV_PIX_FMT_RGB24, nWidth, nHeight);

		//将YUV buffer 填充YUV Frame    
		avpicture_fill((AVPicture*)m_pYUVFrame, (uint8_t*)yuv_buff, AV_PIX_FMT_YUV420P, nWidth, nHeight);

		// 翻转RGB图像    
		m_pRGBFrame->data[0] += m_pRGBFrame->linesize[0] * (nHeight - 1);
		m_pRGBFrame->linesize[0] *= -1;
		m_pRGBFrame->data[1] += m_pRGBFrame->linesize[1] * (nHeight / 2 - 1);
		m_pRGBFrame->linesize[1] *= -1;
		m_pRGBFrame->data[2] += m_pRGBFrame->linesize[2] * (nHeight / 2 - 1);
		m_pRGBFrame->linesize[2] *= -1;


		//将RGB转化为YUV    
		sws_scale(scxt, m_pRGBFrame->data, m_pRGBFrame->linesize, 0, c->height, m_pYUVFrame->data, m_pYUVFrame->linesize);

		int got_packet_ptr = 0;
		av_init_packet(&avpkt);
		avpkt.data = outbuf;
		avpkt.size = outbuf_size;
		u_size = avcodec_encode_video2(c, &avpkt, m_pYUVFrame, &got_packet_ptr);
		m_pYUVFrame->pts++;
		if (u_size == 0)
		{

			//avpkt.pts = av_rescale_q_rnd(avpkt.pts, c->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			//avpkt.dts = av_rescale_q_rnd(avpkt.dts, c->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			//avpkt.duration = av_rescale_q(avpkt.duration, c->time_base, out_stream->time_base);


			time_t tt = time(NULL);//这句返回的只是一个时间cuo
			tm* t = localtime(&tt);

			int64_t i = *((int64_t*)&t);


			avpkt.pts = duation;
			avpkt.dts = duation;
			avpkt.duration = 0;
			duation += 18000;
			avpkt.pos = -1;
			ret = av_interleaved_write_frame(ofmt_ctx, &avpkt);

			if (ret < 0) {
				printf("Error muxing packet %d\n", ret);
			}
			else {
				printf("Error muxing packet ok\n", ret);
			}
			//fwrite(avpkt.data, 1, avpkt.size, f);

		}
	}

	//fclose(h264_f);
	free(outbuf);
	avcodec_close(c);
	av_free(c);

  
	av_write_trailer(ofmt_ctx);
end:
	//avformat_close_input(&ifmt_ctx);

	if (ofmt_ctx && !(ofmt->flags & AVFMT_NOFILE))
		avio_close(ofmt_ctx->pb);
	avformat_free_context(ofmt_ctx);
	if (ret < 0 && ret != AVERROR_EOF) {
		printf("Error occurred.\n");
		return -1;
	}
	return 0;
}
