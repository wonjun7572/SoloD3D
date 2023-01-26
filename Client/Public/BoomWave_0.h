#pragma once

#include "Effect_Mesh.h"

BEGIN(Client)

class CBoomWave_0 final : public CEffect_Mesh
{
private:
	CBoomWave_0(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoomWave_0(const CBoomWave_0& rhs);
	virtual ~CBoomWave_0() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void	Imgui_RenderProperty() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CBoomWave_0* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END