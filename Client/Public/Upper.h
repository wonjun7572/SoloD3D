#pragma once

#include "Parts.h"

BEGIN(Client)

class CUpper final : public CParts
{
private:
	CUpper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUpper(const CUpper& rhs);
	virtual ~CUpper() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components() override;

public:
	void Imgui_RenderProperty() override;

public:
	static CUpper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END