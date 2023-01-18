#pragma once

#include "Effect_Rect.h"

class CExplosionE final : public CEffect_Rect
{
private:
	CExplosionE(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CExplosionE(const CEffect_Rect& rhs);
	virtual ~CExplosionE() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	_float m_fFrame = {0.f};

public:
	static CExplosionE* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

