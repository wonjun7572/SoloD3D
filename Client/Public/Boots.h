#pragma once

#include "Parts.h"

BEGIN(Client)

class CBoots final : public CParts
{
private:
	CBoots(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBoots(const CBoots& rhs);
	virtual ~CBoots() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components() override;

public:
	static CBoots* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END