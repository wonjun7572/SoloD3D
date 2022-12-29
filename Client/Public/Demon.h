#pragma once

#include "Monster.h"

BEGIN(Client)

class CDemon final : public CMonster
{
private:
	CDemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDemon(const CDemon& rhs);
	virtual ~CDemon() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	void Imgui_RenderProperty() override;

public:
	void CollisionToPlayer(_double TimeDelta);
	void CollisionToWeapon(_double TimeDelta);

private:
	void SetUp_FSM() override;
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CDemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END