#include "..\Public\CustomFont.h"

CCustomFont::CCustomFont(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CCustomFont::Init(const wstring & pFontFilePath)
{
	m_pFont = new SpriteFont(m_pDevice, pFontFilePath.c_str());
	m_pSprite = new SpriteBatch(m_pContext);

	return S_OK;
}

HRESULT CCustomFont::Render(const wstring & pText, _fvector vPosition, _fvector vColor)
{
	m_pSprite->Begin();

	m_pFont->DrawString(m_pSprite, pText.c_str(), vPosition, vColor);

	m_pSprite->End();

	return S_OK;
}

CCustomFont * CCustomFont::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & pFontFilePath)
{
	CCustomFont*	pInstance = new CCustomFont(pDevice, pContext);

	if (FAILED(pInstance->Init(pFontFilePath)))
	{
		MSG_BOX("Failed to Created : CCustomFont");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CCustomFont::Free()
{
	Safe_Delete(m_pFont);
	Safe_Delete(m_pSprite);

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
