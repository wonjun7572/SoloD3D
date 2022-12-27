#pragma once

#include "Base.h"

BEGIN(Engine)

class CCustomFont final : public CBase
{
private:
	CCustomFont(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCustomFont() = default;

public:
	HRESULT Init(const wstring& pFontFilePath);
	HRESULT Render(const _tchar* pText, const _float2& vPosition, _float fRadian, _float2 vScale, _fvector vColor);

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	SpriteFont*				m_pFont = nullptr;
	SpriteBatch*			m_pSprite = nullptr;

public:
	static CCustomFont* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& pFontFilePath);
	virtual void Free() override;
};

END