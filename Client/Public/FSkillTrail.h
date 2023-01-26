#pragma once

#include "Effect_Mesh.h"

BEGIN(Client)

class CFSkillTrail final : public CEffect_Mesh
{
private:
	CFSkillTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CFSkillTrail(const CFSkillTrail& rhs);
	virtual ~CFSkillTrail() = default;

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
	static CFSkillTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END