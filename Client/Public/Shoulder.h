#pragma once

#include "Parts.h"

BEGIN(Client)

class CShoulder final : public CParts
{
private:
	CShoulder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShoulder(const CShoulder& rhs);
	virtual ~CShoulder() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components() override;

public:
	static CShoulder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END