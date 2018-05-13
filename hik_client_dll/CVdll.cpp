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
		unsigned short  bfType; //2 λͼ�ļ������ͣ�����Ϊ��BM��  
		unsigned long bfSize; //4 λͼ�ļ��Ĵ�С�����ֽ�Ϊ��λ  
		unsigned short bfReserved1; //2 λͼ�ļ������֣�����Ϊ0  
		unsigned short bfReserved2; //2 λͼ�ļ������֣�����Ϊ0  
		unsigned long bfOffBits; //4 λͼ���ݵ���ʼλ�ã��������λͼ�ļ�ͷ��ƫ������ʾ�����ֽ�Ϊ��λ  
	} BITMAPFILEHEADER;//�ýṹռ��14���ֽڡ�  

	typedef struct tagBITMAPINFOHEADER {
		unsigned long biSize; //4 ���ṹ��ռ���ֽ���  
		long biWidth; //4 λͼ�Ŀ�ȣ�������Ϊ��λ  
		long biHeight; //4 λͼ�ĸ߶ȣ�������Ϊ��λ  
		unsigned short biPlanes; //2 Ŀ���豸��ƽ�������壬����Ϊ1  
		unsigned short biBitCount;//2 ÿ�����������λ����������1(˫ɫ), 4(16ɫ)��8(256ɫ)��24(���ɫ)֮һ  
		unsigned long biCompression; //4 λͼѹ�����ͣ������� 0(��ѹ��),1(BI_RLE8ѹ������)��2(BI_RLE4ѹ������)֮һ  
		unsigned long biSizeImage; //4 λͼ�Ĵ�С�����ֽ�Ϊ��λ  
		long biXPelsPerMeter; //4 λͼˮƽ�ֱ��ʣ�ÿ��������  
		long biYPelsPerMeter; //4 λͼ��ֱ�ֱ��ʣ�ÿ��������  
		unsigned long biClrUsed;//4 λͼʵ��ʹ�õ���ɫ���е���ɫ��  
		unsigned long biClrImportant;//4 λͼ��ʾ��������Ҫ����ɫ��  
	} BITMAPINFOHEADER;//�ýṹռ��40���ֽڡ�  

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
		if ((fp = fopen(csFileName, "rb")) == NULL)  //�Զ����Ƶķ�ʽ���ļ�  
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

		if (fseek(fp, sizeof(BITMAPFILEHEADER), 0))  //����BITMAPFILEHEADE  
		{
			return FALSE;
		}
		BITMAPINFOHEADER infoHead;
		fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);   //��fp�ж�ȡBITMAPINFOHEADER��Ϣ��infoHead��,ͬʱfp��ָ���ƶ�  
		nWidth = infoHead.biWidth;
		nHeight = infoHead.biHeight;
		int linebyte = (nWidth * 24 / 8 + 3) / 4 * 4; //����ÿ�е��ֽ�����24����ͼƬ��24λ��bmpͼ��3��ȷ������ʧ����  

													  //cout<<bmpwidth<<" "<<bmpheight<<endl;  
		nDataLen = linebyte*nHeight;
		pBmpBuf = new unsigned char[linebyte*nHeight];
		fread(pBmpBuf, sizeof(char), linebyte*nHeight, fp);
		fclose(fp);
	}
	printf("file ok\n");
	av_register_all();
	avcodec_register_all();
	AVFrame *m_pRGBFrame = new AVFrame[1];  //RGB֡����      
	AVFrame *m_pYUVFrame = new AVFrame[1];;  //YUV֡����    
	AVCodecContext *c = NULL;
	AVCodecContext *in_c = NULL;
	AVCodec *pCodecH264; //������    
	uint8_t * yuv_buff;//    

					   //����h264������    
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
									//�򿪱�����    
	if (avcodec_open2(c, pCodecH264, NULL)<0)
		printf("���ܴ򿪱����");

	int size = c->width * c->height;

	yuv_buff = (uint8_t *)malloc((size * 3) / 2); // size for YUV 420     

												  //��rgbͼ���������rgb֡    
	uint8_t * rgb_buff = new uint8_t[nDataLen];

	//ͼ�����    
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

	//��ʼ��SwsContext    
	SwsContext * scxt = sws_getContext(c->width, c->height, AV_PIX_FMT_BGR24, c->width, c->height, AV_PIX_FMT_YUV420P, SWS_POINT, NULL, NULL, NULL);

	AVPacket avpkt;

	//-----lbg----------
	AVOutputFormat *ofmt = NULL;
	//�����Ӧһ��AVFormatContext�������Ӧһ��AVFormatContext  
	//��Input AVFormatContext and Output AVFormatContext��  
	AVFormatContext  *ofmt_ctx = NULL;
	//AVPacket pkt;
	const char *in_filename, *out_filename;
	int ret, i;
	int videoindex = -1;
	int frame_index = 0;
	int64_t start_time = 0;

	//in_filename = "F:/FeigeDownload/007.avi";//����URL��Input file URL��  
	//in_filename  = "shanghai03_p.h264";  

	out_filename = "rtsp://192.168.0.104/test1";//��� URL��Output URL��[RTMP]  
												//out_filename = "rtp://233.233.233.233:6666";//��� URL��Output URL��[UDP]  

	av_register_all();
	//Network  
	avformat_network_init();

	//�����Output��  

	avformat_alloc_output_context2(&ofmt_ctx, NULL, "rtsp", out_filename); //RTMP  
																		   //avformat_alloc_output_context2(&ofmt_ctx, NULL, "mpegts", out_filename);//UDP  

	if (!ofmt_ctx) {
		printf("Could not create output context\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	ofmt = ofmt_ctx->oformat;
	//for (i = 0; i < ifmt_ctx->nb_streams; i++) {
	//���������������������Create output AVStream according to input AVStream��  
	//AVStream *in_stream = ifmt_ctx->streams[i];
	//AVStream *out_stream = avformat_new_stream(ofmt_ctx, in_stream->codec->codec);
	AVStream *out_stream = avformat_new_stream(ofmt_ctx, pCodecH264);
	if (!out_stream) {
		printf("Failed allocating output stream\n");
		ret = AVERROR_UNKNOWN;
		goto end;
	}
	//����AVCodecContext�����ã�Copy the settings of AVCodecContext��  
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
	//�����URL��Open output URL��  
	if (!(ofmt->flags & AVFMT_NOFILE)) {
		ret = avio_open(&ofmt_ctx->pb, out_filename, AVIO_FLAG_WRITE);
		if (ret < 0) {
			printf("Could not open output URL '%s'", out_filename);
			goto end;
		}
	}
	//д�ļ�ͷ��Write file header��  

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
		if ((fp = fopen(csFileName, "rb")) == NULL)  //�Զ����Ƶķ�ʽ���ļ�  
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

		if (fseek(fp, sizeof(BITMAPFILEHEADER), 0))  //����BITMAPFILEHEADE  
		{
			return FALSE;
		}
		BITMAPINFOHEADER infoHead;
		fread(&infoHead, sizeof(BITMAPINFOHEADER), 1, fp);   //��fp�ж�ȡBITMAPINFOHEADER��Ϣ��infoHead��,ͬʱfp��ָ���ƶ�  
		nWidth = infoHead.biWidth;
		nHeight = infoHead.biHeight;
		int linebyte = (nWidth * 24 / 8 + 3) / 4 * 4; //����ÿ�е��ֽ�����24����ͼƬ��24λ��bmpͼ��3��ȷ������ʧ����  

													  //cout<<bmpwidth<<" "<<bmpheight<<endl;  
		nDataLen = linebyte*nHeight;
		pBmpBuf = new unsigned char[linebyte*nHeight];
		fread(pBmpBuf, sizeof(char), linebyte*nHeight, fp);
		fclose(fp);
		memcpy(rgb_buff, pBmpBuf, nDataLen);


		avpicture_fill((AVPicture*)m_pRGBFrame, (uint8_t*)rgb_buff, AV_PIX_FMT_RGB24, nWidth, nHeight);

		//��YUV buffer ���YUV Frame    
		avpicture_fill((AVPicture*)m_pYUVFrame, (uint8_t*)yuv_buff, AV_PIX_FMT_YUV420P, nWidth, nHeight);

		// ��תRGBͼ��    
		m_pRGBFrame->data[0] += m_pRGBFrame->linesize[0] * (nHeight - 1);
		m_pRGBFrame->linesize[0] *= -1;
		m_pRGBFrame->data[1] += m_pRGBFrame->linesize[1] * (nHeight / 2 - 1);
		m_pRGBFrame->linesize[1] *= -1;
		m_pRGBFrame->data[2] += m_pRGBFrame->linesize[2] * (nHeight / 2 - 1);
		m_pRGBFrame->linesize[2] *= -1;


		//��RGBת��ΪYUV    
		sws_scale(scxt, m_pRGBFrame->data, m_pRGBFrame->linesize, 0, c->height, m_pYUVFrame->data, m_pYUVFrame->linesize);

		int got_packet_ptr = 0;
		av_init_packet(&avpkt);
		avpkt.data = outbuf;
		avpkt.size = outbuf_size;
		u_size = avcodec_encode_video2(c, &avpkt, m_pYUVFrame, &got_packet_ptr);
		if (u_size == 0)
		{

			//avpkt.pts = av_rescale_q_rnd(avpkt.pts, c->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			//avpkt.dts = av_rescale_q_rnd(avpkt.dts, c->time_base, out_stream->time_base, (AVRounding)(AV_ROUND_NEAR_INF | AV_ROUND_PASS_MINMAX));
			//avpkt.duration = av_rescale_q(avpkt.duration, c->time_base, out_stream->time_base);


			time_t tt = time(NULL);//��䷵�ص�ֻ��һ��ʱ��cuo
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