#pragma once
#include "Engine_Define.h"

class ENGINE_DLL CGameUtils
{
private:
	CGameUtils() = default;
	
public:
	static void wc2c(const wchar_t* szWchar, char* szChar);		
	static void c2wc(const char* szChar, wchar_t* szWchar);
};

