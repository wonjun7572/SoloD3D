#pragma once

#include "Client_Define.h"
#include "UI.h"

BEGIN(Client)

class CMonsterNameUI final : public CUI
{
public:
	typedef struct tagDamageFont
	{
		_tchar szName[MAX_PATH];
		_tchar szFontName[MAX_PATH];
		_float4 vColor;

		_float	fX;
		_float	fY;
		_float	fSizeX;
		_float	fSizeY;
	}NAMEFONT;

private:
	CMonsterNameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterNameUI(const CMonsterNameUI& rhs);
	virtual	~CMonsterNameUI() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	NAMEFONT m_tagNameFont;

private:
	HRESULT SetUp_Components();

public:
	static CMonsterNameUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END