/*
*     COPYRIGHT NOTICE
*     Copyright(c) 2017, Alopex/Helium
*     All rights reserved.
*
* @file		DynamicWall.h
* @brief	This Program is DynamicWall Project.
* @author	Alopex/Helium
* @version	v1.00a
* @date		2017-11-29
*/
#pragma once

extern bool g_bDecodeFlag;
extern CRITICAL_SECTION g_csDecode;

extern unsigned char* g_pArrayY;
extern unsigned char* g_pArrayU;
extern unsigned char* g_pArrayV;

extern int g_nVideoWidth;
extern int g_nVideoHeight;

extern BOOL DynamicWallInit();
extern void DynamicWallRelease();
extern void DynamicWallUpdate();
extern void DynamicWallRender();
