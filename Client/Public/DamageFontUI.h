#pragma once

#include "Client_Define.h"
#include "UI.h"

BEGIN(Client)

class CDamageFontUI final : public CUI
{
public:
	typedef struct tagDamageFont
	{
		_tchar szDamage[MAX_PATH];
		_tchar szFontName[MAX_PATH];
		_float4 vColor;
		_float4 vPos;
		_uint	iVersion;
	}DAMAGEFONT;

private:
	CDamageFontUI(ID3D11Device* pDeviec, ID3D11DeviceContext* pContext);
	CDamageFontUI(const CDamageFontUI& rhs);
	virtual	~CDamageFontUI() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	DAMAGEFONT m_tagDamageFont;

private:
	HRESULT SetUp_Components();

	_double m_TimeDelta = 0.0;
	_float	m_fSize = 0.f;

public:
	static CDamageFontUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END