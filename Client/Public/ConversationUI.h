#pragma once

#include "Client_Define.h"
#include "UI.h"

BEGIN(Client)

class CConversationUI final : public CUI
{
public:
	typedef struct tagConversationFont
	{
		_tchar szConversation[MAX_PATH];
		_tchar szFontName[MAX_PATH];
		_float4 vColor;

		_float	fX;
		_float	fY;
		_float	fSizeX;
		_float	fSizeY;
		_bool	bTextureOn;
	}CONVERSATIONFONT;

private:
	CConversationUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CConversationUI(const CConversationUI& rhs);
	virtual	~CConversationUI() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void	SetTransform(const CONVERSATIONFONT& tagFont);
	void	SetConversation(const wstring& strConversation);

	CONVERSATIONFONT m_tagConversationFont;

	void	Imgui_RenderProperty() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	_float	m_fTextureX = 0.f, m_fTextureY = 0.f;
	_float	m_fTextureCX = 1.f, m_fTextureCY = 1.f;

	_float	m_fAlpha = 0.f;

public:
	static CConversationUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END