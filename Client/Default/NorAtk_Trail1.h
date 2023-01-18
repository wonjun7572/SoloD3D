#pragma once

#include "Effect_Mesh.h"

BEGIN(Client)

class CNorAtk_Trail1 final : public CEffect_Mesh
{
private:
	CNorAtk_Trail1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNorAtk_Trail1(const CNorAtk_Trail1& rhs);
	virtual ~CNorAtk_Trail1() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CNorAtk_Trail1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END