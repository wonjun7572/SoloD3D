#pragma once

#include "Effect_Mesh.h"

BEGIN(Client)

class CTwister_Line final : public CEffect_Mesh
{
private:
	CTwister_Line(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTwister_Line(const CTwister_Line& rhs);
	virtual ~CTwister_Line() = default;

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
	static CTwister_Line* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END