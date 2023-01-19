#pragma once

#include "Effect_Rect.h"

class CMonsterHpUI final : public CEffect_Rect
{
private:
	CMonsterHpUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonsterHpUI(const CEffect_Rect& rhs);
	virtual ~CMonsterHpUI() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void	Set_Amount(_float fAmount) { m_fAmount = fAmount; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	_float		m_fAmount = 0.f;

public:
	static CMonsterHpUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

