#pragma once

#include "Effect_Rect.h"

class CTorchFlame final : public CEffect_Rect
{
private:
	CTorchFlame(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTorchFlame(const CEffect_Rect& rhs);
	virtual ~CTorchFlame() = default;

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

	_float m_fFrame = { 0.f };
	_float m_CSize[3] = { 1.f, 1.f , 1.f };

	_bool m_bLoadLight = false;
	
public:
	static CTorchFlame* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

