/*
*     COPYRIGHT NOTICE
*     Copyright(c) 2017, Alopex/Helium
*     All rights reserved.
*
* @file		WinProcess.h
* @brief	This Program is WinProcess DLL Project.
* @author	Alopex/Helium
* @version	v1.00a
* @date		2017-11-29
*/
#pragma once

#ifndef __WINPROCESS_H_
#define __WINPROCESS_H_

//Include Header File
#include "WinUtilities.h"

//Macro Definition
#ifndef USER_SCREENWIDTH
#define USER_SCREENWIDTH	1440				//窗口宽度
#else
#undef USER_SCREENWIDTH
#define USER_SCREENWIDTH	1440				//窗口宽度
#endif

#ifndef USER_SCREENHEIGHT
#define USER_SCREENHEIGHT	900				//窗口高度
#else
#undef USER_SCREENHEIGHT
#define USER_SCREENHEIGHT	900				//窗口宽度
#endif

#ifndef USER_WINDOWTITLE
#define USER_WINDOWTITLE	L"DynamicWall"	//窗口标题名称
#endif

#ifndef USER_WINDOWICON
#define USER_WINDOWICON		(WORD)0			//窗口标题图标
#endif

//Function Definition
extern WndPara* SetWindowParameterCallBack(void);
extern BOOL InitWindowExtraCallBack(void);
extern void ReleaseWindowExtraCallBack(void);
extern void Direct3DRenderCallBack(void);

#endif