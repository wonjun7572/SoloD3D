#pragma once

#include "Enemy.h"

BEGIN(Client)

class CLightTank final : public CEnemy
{
private:
	CLightTank(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightTank(const CLightTank& rhs);
	virtual ~CLightTank() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Imgui_RenderProperty() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CLightTank* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END