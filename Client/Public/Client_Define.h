#pragma once

namespace Client
{
	enum LEVEL { LEVEL_LOGO, LEVEL_LOADING, LEVEL_CHAP1 , LEVEL_CHAP2, LEVEL_CHAP3, LEVEL_TOOL , LEVEL_END };
	enum SOUNDCHANNEL	{SOUND_BGM, SOUND_PLAYER, SOUND_END	};
}

extern HWND			g_hWnd;
extern HINSTANCE	g_hInst;
extern bool			g_bFullScreen;
extern bool			g_bNeedResizeSwapChain;
extern unsigned int	g_iWinSizeX;
extern unsigned int	g_iWinSizeY;

extern unsigned int	g_LEVEL;

using namespace Client;

#include "MathUtils.h"