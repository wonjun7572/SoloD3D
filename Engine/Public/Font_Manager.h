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
	HRESULT Render_Font(const wstring& pFontTag, const wstring& pText, _fvector vPos, _fvector vColor);

private:
	map<wstring, class CCustomFont*>			m_Fonts;
	typedef map<wstring, class CCustomFont*>	FONTS;

private:
	class CCustomFont* Find_Fonts(const wstring& pFontTag);

public:
	virtual void Free() override;
};

END