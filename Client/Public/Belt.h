#pragma once

#include "Parts.h"

BEGIN(Client)

class CBelt final : public CParts
{
private:
	CBelt(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBelt(const CBelt& rhs);
	virtual ~CBelt() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components() override;
	HRESULT SetUp_ShaderResources() override;

public:
	void Imgui_RenderProperty() override;

public:
	static CBelt* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END