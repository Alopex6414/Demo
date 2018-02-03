/*
*     COPYRIGHT NOTICE
*     Copyright(c) 2017, Alopex/Helium
*     All rights reserved.
*
* @file		DynamicWall.cpp
* @brief	This Program is DynamicWall Project.
* @author	Alopex/Helium
* @version	v1.00a
* @date		2017-11-29
*/
#include "DirectCommon.h"
#include "DirectGraphics.h"
#include "DirectSurface.h"
#include "DirectThreadSafe.h"
#include "WinProcess.h"
#include "WinUtilities.h"
#include "Cherryfps.h"
#include "DynamicWall.h"
#include "DecodeThread.h"

IDirect3DDevice9* g_pD3D9Device = NULL;
IDirect3DSurface9* g_pD3D9Surface = NULL;

DirectGraphics* g_pMainGraphics = NULL;
DirectSurface* g_pMainSurface = NULL;

CCherryfps* g_pMainfps = NULL;

CDecodeThread g_cDecodeThread;
CPlumThread* g_pPlumThread = NULL;

bool g_bDecodeFlag = false;
CRITICAL_SECTION g_csDecode;

unsigned char* g_pArrayY = NULL;
unsigned char* g_pArrayU = NULL;
unsigned char* g_pArrayV = NULL;

int g_nVideoWidth = 0;
int g_nVideoHeight = 0;

//Test...
HWND g_hDeskTop;
int g_nDeskTopWidth = 0;
int g_nDeskTopHeight = 0;


// DynamicWallInit初始化
BOOL DynamicWallInit()
{
	HRESULT hr;

	//获取桌面窗口句柄
	g_nDeskTopWidth = GetSystemMetrics(SM_CXSCREEN);
	g_nDeskTopHeight = GetSystemMetrics(SM_CYSCREEN);

	g_hDeskTop = FindWindowEx(GetDesktopWindow(), NULL, L"Progman", L"Program Manager");
	g_hDeskTop = FindWindowEx(g_hDeskTop, NULL, L"SHELLDLL_DefView", 0);
	g_hDeskTop = FindWindowEx(g_hDeskTop, NULL, L"SysListView32", L"FolderView");

	//初始化Direct3D
	g_pMainGraphics = new DirectGraphics;
	hr = g_pMainGraphics->DirectGraphicsInit(g_hWnd, true, USER_SCREENWIDTH, USER_SCREENHEIGHT);
	if (FAILED(hr))
	{
		MessageBox(g_hWnd, L"Direct3D初始化失败!", L"错误", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//初始化Direct3DFont
	hr = g_pMainGraphics->DirectGraphicsFontInit(20);
	if (FAILED(hr))
	{
		MessageBox(g_hWnd, L"Direct3DFont初始化失败!", L"错误", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	g_pD3D9Device = g_pMainGraphics->DirectGraphicsGetDevice();

	//初始化Direct3DSurface
	g_pMainSurface = new DirectSurface(g_pD3D9Device);
	g_pD3D9Surface = g_pMainSurface->DirectSurfaceGetSurface();

	hr = g_pD3D9Device->CreateOffscreenPlainSurface(1920, 1080, (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'), D3DPOOL_DEFAULT, &g_pD3D9Surface, NULL);
	if (FAILED(hr))
	{
		MessageBox(g_hWnd, L"Direct3DSurface初始化失败!", L"错误", MB_OK | MB_ICONERROR);
		return FALSE;
	}

	//初始化FPS
	g_pMainfps = new CCherryfps(g_pD3D9Device);
	g_pMainfps->CherryfpsInit(20, (LPWSTR)_T("Consolas"));

	//初始化临界区
	InitializeCriticalSection(&g_csDecode);

	//初始化线程DecodeThread
	g_pPlumThread = new CPlumThread(&g_cDecodeThread);
	g_pPlumThread->PlumThreadInit();

	return TRUE;
}

// DynamicWallRelease释放
void DynamicWallRelease()
{
	//删除临界区
	DeleteCriticalSection(&g_csDecode);

	SAFE_DELETE_ARRAY(g_pArrayY);
	SAFE_DELETE_ARRAY(g_pArrayU);
	SAFE_DELETE_ARRAY(g_pArrayV);

	if (g_pPlumThread)
	{
		g_pPlumThread->PlumThreadExit();
		SAFE_DELETE(g_pPlumThread);
	}

	SAFE_DELETE(g_pMainfps);
	SAFE_DELETE(g_pMainSurface);
	SAFE_DELETE(g_pMainGraphics);
}

//DynamicWallUpdate刷新
void DynamicWallUpdate()
{
	D3DLOCKED_RECT Rect;

	if (g_bDecodeFlag)
	{
		EnterCriticalSection(&g_csDecode);

		HRESULT hr;

		hr = g_pD3D9Device->TestCooperativeLevel();
		if (hr != S_OK)
		{
			if (hr == D3DERR_DEVICELOST)
			{
				LeaveCriticalSection(&g_csDecode);
				return;
			}

			if (hr == D3DERR_DEVICENOTRESET)
			{
				ID3DXFont* pD3DXFont = NULL;
				D3DPRESENT_PARAMETERS D3D9pp;

				pD3DXFont = (ID3DXFont*)g_pMainGraphics->DirectGraphicsGetFont();
				hr = pD3DXFont->OnLostDevice();

				DirectFont* pDirectFont = NULL;
				pDirectFont = g_pMainfps->CherryfpsGetDirectFont();
				pD3DXFont = pDirectFont->DirectFontGetFont();
				pD3DXFont->OnLostDevice();

				SAFE_RELEASE(g_pD3D9Surface);

				IDirect3DSurface9* g_pD3D9BackBuffer = NULL;
				g_pD3D9Device->GetBackBuffer(NULL, NULL, D3DBACKBUFFER_TYPE_MONO, &g_pD3D9BackBuffer);
				SAFE_RELEASE(g_pD3D9BackBuffer);

				D3D9pp.BackBufferWidth = USER_SCREENWIDTH;//后台缓冲表面宽度(Pixel)
				D3D9pp.BackBufferHeight = USER_SCREENHEIGHT;//后台缓冲表面高度(Pixel)
				D3D9pp.BackBufferFormat = D3DFMT_UNKNOWN;//后台缓冲像素格式
				D3D9pp.BackBufferCount = 1;//后台缓冲数量(1)
				D3D9pp.MultiSampleType = D3DMULTISAMPLE_NONE;//后台缓冲多重采样类型
				D3D9pp.MultiSampleQuality = 0;//后台缓冲多重采样质量
				D3D9pp.SwapEffect = D3DSWAPEFFECT_DISCARD;//交换链页面置换方式
				D3D9pp.hDeviceWindow = g_hWnd;//设备相关窗口句柄
				D3D9pp.Windowed = true;//窗口模式:true/全屏模式:false
				D3D9pp.EnableAutoDepthStencil = true;//Direct3D自动创建维护深度缓冲和模板缓冲
				D3D9pp.AutoDepthStencilFormat = D3DFMT_D24S8;//深度缓冲和模板缓冲像素格式
				D3D9pp.Flags = 0;//无标记格式
				D3D9pp.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;//Direct3D默认刷新频率
				D3D9pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;//Direct3D提交频率(默认提交)

				hr = g_pD3D9Device->Reset(&D3D9pp);

				hr = g_pD3D9Device->CreateOffscreenPlainSurface(1920, 1080, (D3DFORMAT)MAKEFOURCC('Y', 'V', '1', '2'), D3DPOOL_DEFAULT, &g_pD3D9Surface, NULL);
			}

		}

		g_pD3D9Surface->LockRect(&Rect, 0, 0);

		for (int i = 0; i < g_nVideoHeight; ++i)
		{
			memcpy((BYTE*)(Rect.pBits) + i * Rect.Pitch, (BYTE*)g_pArrayY + i * g_nVideoWidth, g_nVideoWidth);
		}

		for (int i = 0; i < g_nVideoHeight / 2; ++i)
		{
			memcpy((BYTE*)(Rect.pBits) + Rect.Pitch * g_nVideoHeight + (Rect.Pitch / 2) * i, (BYTE*)g_pArrayV + i * g_nVideoWidth / 2, g_nVideoWidth / 2);
		}

		for (int i = 0; i < g_nVideoHeight / 2; ++i)
		{
			memcpy((BYTE*)(Rect.pBits) + Rect.Pitch * g_nVideoHeight + Rect.Pitch * g_nVideoHeight / 4 + (Rect.Pitch / 2) * i, (BYTE*)g_pArrayU + i * g_nVideoWidth / 2, g_nVideoWidth / 2);
		}

		g_pD3D9Surface->UnlockRect();

		g_bDecodeFlag = false;

		LeaveCriticalSection(&g_csDecode);
	}

}

// DynamicWallRender渲染
void DynamicWallRender()
{
	IDirect3DSurface9* g_pD3D9BackBuffer = NULL;
	D3DSURFACE_DESC Desc;
	RECT SrcRect;
	RECT DestRect;

	g_pMainGraphics->DirectGraphicsBegin();

	g_pD3D9Surface->GetDesc(&Desc);

	SrcRect.left = 0;
	SrcRect.top = 0;
	SrcRect.right = g_nVideoWidth;
	SrcRect.bottom = g_nVideoHeight;

	DestRect.left = 0;
	DestRect.top = 0;
	DestRect.right = USER_SCREENWIDTH;
	DestRect.bottom = USER_SCREENHEIGHT;

	g_pD3D9Device->GetBackBuffer(NULL, NULL, D3DBACKBUFFER_TYPE_MONO, &g_pD3D9BackBuffer);
	g_pD3D9Device->StretchRect(g_pD3D9Surface, &SrcRect, g_pD3D9BackBuffer, &DestRect, D3DTEXF_NONE);

	g_pMainfps->CherryfpsGetfps();
	g_pMainfps->CherryfpsDrawfps(g_hWnd, DIRECTFONT_FORMAT_TOPRIGHT, D3DXCOLOR(1.0f, 0.5f, 0.5f, 1.0f));
	g_pMainGraphics->DirectGraphicsFontDrawText(g_hWnd);

	g_pMainGraphics->DirectGraphicsEnd();
	SAFE_RELEASE(g_pD3D9BackBuffer);
}