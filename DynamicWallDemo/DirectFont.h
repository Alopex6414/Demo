/*
*     COPYRIGHT NOTICE
*     Copyright(c) 2017, Team Shanghai Dream Equinox
*     All rights reserved.
*
* @file		DirectFont.h
* @brief	This File is DirectFont DLL Project.
* @author	Alopex/Helium
* @version	v1.11a
* @date		2017-12-16	v1.00a	alopex	Create This File.
* @date		2018-1-10	v1.10a	alopex	Code Add dxerr & d3dcompiler Library and Modify Verify.
* @date		2018-1-10	v1.10a	alopex	Add Thread Safe File & Variable(DirectThreadSafe).
*/
#pragma once

#ifndef __DIRECTFONT_H_
#define __DIRECTFONT_H_

//Include DirectX Common Header File
#include "DirectCommon.h"

//Macro Definition
#ifdef	DIRECTFONT_EXPORTS
#define DIRECTFONT_API	__declspec(dllexport)
#else
#define DIRECTFONT_API	__declspec(dllimport)
#endif

#define DIRECTFONT_FORMAT_CENTER		(DT_CENTER)
#define DIRECTFONT_FORMAT_TOPLEFT		(DT_TOP | DT_LEFT)
#define DIRECTFONT_FORMAT_TOPRIGHT		(DT_TOP | DT_RIGHT)
#define DIRECTFONT_FORMAT_BOTTOMLEFT	(DT_BOTTOM | DT_LEFT)
#define DIRECTFONT_FORMAT_BOTTOMRIGHT	(DT_BOTTOM | DT_RIGHT)

//Class Definition
class DirectFont
{
private:
	IDirect3DDevice9* m_pD3D9Device;			//The Direct3D 9 Device(D3D9绘制设备)
	ID3DXFont* m_pD3D9Font;						//The Direct3D 9 Font(D3D9绘制字体)

	CRITICAL_SECTION m_cs;						//Thread Safe(CriticalSection)
	bool m_bThreadSafe;							//Thread Safe Status

public:
	DirectFont();								//DirectFont Constructor Function(构造函数)
	~DirectFont();								//DirectFont Destructor  Function(析构函数)

	//构造
	DirectFont(IDirect3DDevice9* pD3D9Device);	//DirectFont Constructor Function(构造函数)

	//访问
	IDirect3DDevice9* WINAPI DirectFontGetDevice(void) const;			//DirectFont Get D3D9 Device(获取D3D9设备)
	ID3DXFont* WINAPI DirectFontGetFont(void) const;					//DirectFont Get D3D9 Font(获取D3D9字体)

	//控制
	void WINAPI DirectFontSetDevice(IDirect3DDevice9* pD3D9Device);		//DirectFont Set D3D9 Device(设置D3D9设备)
	void WINAPI DirectFontSetFont(ID3DXFont* pD3DXFont);				//DirectFont Set D3DX Font(设置D3D9字体)

	//重置
	virtual void WINAPI DirectFontReset(void);							//DirectFont Reset D3DX Font(重置D3DX字体)

	//初始化
	virtual HRESULT WINAPI DirectFontInit(void);										//DirectFont Create Font(DirectFont初始化)
	virtual HRESULT WINAPI DirectFontInit(int nFontSize);								//DirectFont Create Font(DirectFont初始化)
	virtual HRESULT WINAPI DirectFontInit(int nFontSize, LPWSTR lpszFontType);			//DirectFont Create Font(DirectFont初始化)

	//绘制
	virtual void WINAPI DirectFontDrawText(HWND hWnd, LPCWSTR lpcszStr, DWORD Format, D3DCOLOR Color);			//DirectFont Draw Text(DirectFont绘制)

};

#endif