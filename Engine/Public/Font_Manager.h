#pragma once

#include "Base.h"

BEGIN(Engine)

class CFont_Manager final : public CBase
{
	DECLARE_SINGLETON(CFont_Manager)
public:
	CFont_Manager();
	virtual ~CFont_Manager() = default;

public:
	HRESULT Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& pFontTag, const wstring& pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vScale);
	void	DrawTextInWorld(const _tchar* pFontTag, const _tchar* text, _float4 vPos, _float3 vScale, _float4 vColor);

private:
	map<wstring, class CCustomFont*>			m_Fonts;
	typedef map<wstring, class CCustomFont*>	FONTS;

private:
	class CCustomFont* Find_Fonts(const wstring& pFontTag);

public:
	virtual void Free() override;
};

END