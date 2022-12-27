#include "..\Public\Font_Manager.h"
#include "CustomFont.h"

IMPLEMENT_SINGLETON(CFont_Manager)

CFont_Manager::CFont_Manager()
{
}

HRESULT CFont_Manager::Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & pFontTag, const wstring & pFontFilePath)
{
	if (nullptr != Find_Fonts(pFontTag))
		return E_FAIL;

	CCustomFont*		pFont = CCustomFont::Create(pDevice, pContext, pFontFilePath);

	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.insert({ pFontTag, pFont });

	return S_OK;
};

HRESULT CFont_Manager::Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor)
{
	CCustomFont*		pFont = Find_Fonts(pFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(pText, vPos, fRadian, vScale, vColor);
}

CCustomFont * CFont_Manager::Find_Fonts(const wstring & pFontTag)
{
	auto iter = m_Fonts.find(pFontTag);

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();
}
