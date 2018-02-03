/*
*     COPYRIGHT NOTICE
*     Copyright(c) 2017, Alopex/Helium
*     All rights reserved.
*
* @file		DecodeThread.cpp
* @brief	This Program is DynamicWall Project.
* @author	Alopex/Helium
* @version	v1.00a
* @date		2017-11-29
*/
#include "FFmpeg.h"
#include "DecodeThread.h"
#include "WinUtilities.h"
#include "DynamicWall.h"

#pragma warning (disable:4996)

CDecodeThread::CDecodeThread()
{
}

CDecodeThread::~CDecodeThread()
{
}

void WINAPI CDecodeThread::PlumThreadRun()
{
	AVFormatContext *pFormatCtx;
	AVCodecContext  *pCodecCtx;
	AVCodec         *pCodec;
	AVFrame			*pFrame;
	AVFrame			*pFrameYUV;
	char* pTemp = NULL;
	char FilePath[] = "Everybody.mp4";//YourName.mov
	char ModulePath[MAX_PATH] = {0};

	GetModuleFileNameA(NULL, ModulePath, MAX_PATH);
	pTemp = strrchr(ModulePath, '\\') + 1;
	strcpy_s(pTemp, sizeof(FilePath), FilePath);

	av_register_all();//注册所有组件
	avformat_network_init();//初始化网络
	pFormatCtx = avformat_alloc_context();//初始化一个AVFormatContext

	//打开输入的视频文件
	if (avformat_open_input(&pFormatCtx, ModulePath, NULL, NULL) != 0)
	{
		MessageBox(g_hWnd, L"打开文件失败!", L"错误", MB_OK | MB_ICONERROR);
		return;
	}

	//获取视频文件信息
	if (avformat_find_stream_info(pFormatCtx, NULL) < 0) 
	{
		MessageBox(g_hWnd, L"无法读取文件信息!", L"错误", MB_OK | MB_ICONERROR);
		return;
	}

	int VideoIndex = -1;

	for (int i = 0; i < pFormatCtx->nb_streams; i++)
	{
		if (pFormatCtx->streams[i]->codec->codec_type == AVMEDIA_TYPE_VIDEO) 
		{
			VideoIndex = i;
			break;
		}
	}

	if (VideoIndex == -1) 
	{
		MessageBox(g_hWnd, L"未读取到视频信息!", L"错误", MB_OK | MB_ICONERROR);
		return;
	}

	pCodecCtx = pFormatCtx->streams[VideoIndex]->codec;

	//查找解码器
	pCodec = avcodec_find_decoder(pCodecCtx->codec_id);
	if (pCodec == NULL) 
	{
		MessageBox(g_hWnd, L"未查找到解码器!", L"错误", MB_OK | MB_ICONERROR);
		return;
	}

	//打开解码器
	if (avcodec_open2(pCodecCtx, pCodec, NULL) < 0)
	{
		MessageBox(g_hWnd, L"无法打开解码器!", L"错误", MB_OK | MB_ICONERROR);
		return;
	}

	uint8_t *OutBuffer;
	AVPacket *Packet;

	pFrame = av_frame_alloc();
	pFrameYUV = av_frame_alloc();

	OutBuffer = (uint8_t *)av_malloc(avpicture_get_size(AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height));
	avpicture_fill((AVPicture *)pFrameYUV, OutBuffer, AV_PIX_FMT_YUV420P, pCodecCtx->width, pCodecCtx->height);
	Packet = (AVPacket *)av_malloc(sizeof(AVPacket));

	int nSize = pCodecCtx->width * pCodecCtx->height;

	g_nVideoWidth = pCodecCtx->width;
	g_nVideoHeight = pCodecCtx->height;

	g_pArrayY = new unsigned char[nSize];
	g_pArrayU = new unsigned char[nSize];
	g_pArrayV = new unsigned char[nSize];
	memset(g_pArrayY, 0, nSize);
	memset(g_pArrayU, 0, nSize);
	memset(g_pArrayV, 0, nSize);

	av_dump_format(pFormatCtx, 0, ModulePath, 0);

	//读取一帧压缩数据
	int ret;
	int got_picture;
	int y_size;
	struct SwsContext *img_convert_ctx;

	img_convert_ctx = sws_getContext(pCodecCtx->width, pCodecCtx->height, pCodecCtx->pix_fmt, pCodecCtx->width, pCodecCtx->height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	while (true)
	{

		while (av_read_frame(pFormatCtx, Packet) >= 0)
		{
			if (Packet->stream_index == VideoIndex)
			{
				ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, Packet);//解码一帧压缩数据
				if (ret < 0)
				{
					return;
				}
				if (got_picture)
				{
					sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
					y_size = pCodecCtx->width*pCodecCtx->height;

					EnterCriticalSection(&g_csDecode);
					memset(g_pArrayY, 0, nSize);
					memset(g_pArrayU, 0, nSize);
					memset(g_pArrayV, 0, nSize);
					memcpy_s(g_pArrayY, nSize, pFrameYUV->data[0], y_size);
					memcpy_s(g_pArrayU, nSize, pFrameYUV->data[1], y_size / 4);
					memcpy_s(g_pArrayV, nSize, pFrameYUV->data[2], y_size / 4);
					g_bDecodeFlag = true;
					LeaveCriticalSection(&g_csDecode);
				}
			}
			av_free_packet(Packet);
		}

		while (true)
		{
			ret = avcodec_decode_video2(pCodecCtx, pFrame, &got_picture, Packet);
			if (ret < 0)
			{
				break;
			}

			if (!got_picture)
			{
				break;
			}

			sws_scale(img_convert_ctx, (const uint8_t* const*)pFrame->data, pFrame->linesize, 0, pCodecCtx->height, pFrameYUV->data, pFrameYUV->linesize);
			y_size = pCodecCtx->width*pCodecCtx->height;

			EnterCriticalSection(&g_csDecode);
			memset(g_pArrayY, 0, nSize);
			memset(g_pArrayU, 0, nSize);
			memset(g_pArrayV, 0, nSize);
			memcpy_s(g_pArrayY, nSize, pFrameYUV->data[0], y_size);
			memcpy_s(g_pArrayU, nSize, pFrameYUV->data[1], y_size / 4);
			memcpy_s(g_pArrayV, nSize, pFrameYUV->data[2], y_size / 4);
			g_bDecodeFlag = true;
			LeaveCriticalSection(&g_csDecode);
		}

		ret = av_seek_frame(pFormatCtx, VideoIndex, pFormatCtx->streams[VideoIndex]->start_time, 0);
		if (ret < 0)
		{
			break;
		}

		avcodec_flush_buffers(pFormatCtx->streams[VideoIndex]->codec);
	}

	sws_freeContext(img_convert_ctx);

	av_frame_free(&pFrameYUV);
	av_frame_free(&pFrame);
	avcodec_close(pCodecCtx);
	avformat_close_input(&pFormatCtx);

}