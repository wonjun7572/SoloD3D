#pragma once

#include "Effect_Mesh.h"

BEGIN(Client)

class CBoomWave_1 final : public CEffect_Mesh
{
private:
	CBoomWave_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoomWave_1(const CBoomWave_1& rhs);
	virtual ~CBoomWave_1() = default;

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
	static CBoomWave_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END