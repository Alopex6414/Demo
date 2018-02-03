/*
*     COPYRIGHT NOTICE
*     Copyright(c) 2017, Alopex/Helium
*     All rights reserved.
*
* @file		FFmpeg.h
* @brief	This Program is DynamicWall Project.
* @author	Alopex/Helium
* @version	v1.00a
* @date		2017-11-29
*/
#pragma once

extern "C"
{
	#include "include\libavcodec\avcodec.h"  
	#include "include\libavformat\avformat.h"  
	#include "include\libavutil\channel_layout.h"  
	#include "include\libavutil\common.h"  
	#include "include\libavutil\imgutils.h"  
	#include "include\libswscale\swscale.h"   
	#include "include\libavutil\imgutils.h"      
	#include "include\libavutil\opt.h"         
	#include "include\libavutil\mathematics.h"      
	#include "include\libavutil\samplefmt.h"   
};
#pragma comment(lib, "avcodec.lib")  
#pragma comment(lib, "avformat.lib")  
#pragma comment(lib, "avdevice.lib")  
#pragma comment(lib, "avfilter.lib")  
#pragma comment(lib, "avutil.lib")  
#pragma comment(lib, "postproc.lib")  
#pragma comment(lib, "swresample.lib")  
#pragma comment(lib, "swscale.lib") 
