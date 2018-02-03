/*
*     COPYRIGHT NOTICE
*     Copyright(c) 2017, Alopex/Helium
*     All rights reserved.
*
* @file		DecodeThread.h
* @brief	This Program is DynamicWall Project.
* @author	Alopex/Helium
* @version	v1.00a
* @date		2017-11-29
*/
#pragma once

#include "PlumThread.h"

class CDecodeThread : public CPlumThreadBase
{
public:
	CDecodeThread();
	~CDecodeThread();
	virtual void WINAPI PlumThreadRun();
};
