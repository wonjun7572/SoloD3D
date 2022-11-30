#include "..\Public\GameUtils.h"
#include "Engine_Define.h"

void CGameUtils::wc2c(const wchar_t * szWchar, char * szChar)
{
	_int len;
	_int slength = lstrlen(szWchar) + 1;
	len = ::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, 0, 0, 0, 0);
	::WideCharToMultiByte(CP_ACP, 0, szWchar, slength, szChar, len, 0, 0);
}

void CGameUtils::c2wc(const char * szChar, wchar_t * szWchar)
{
	_int len;
	size_t slength = strlen(szChar) + 1;
	len = ::MultiByteToWideChar(CP_ACP, 0, szChar, -1, 0, 0);
	::MultiByteToWideChar(CP_ACP, 0, szChar,static_cast<int>(slength), szWchar, len);
}
