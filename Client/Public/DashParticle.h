#pragma once

#include "Effect_Point.h"

class CDashParticle final : public Effect_Point
{
private:
	CDashParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDashParticle(const CDashParticle& rhs);
	virtual ~CDashParticle() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT	SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CDashParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

