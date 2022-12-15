#pragma once

#include "Enemy.h"

BEGIN(Client)

class CTechSoldier final : public CEnemy
{
private:
	CTechSoldier(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTechSoldier(const CTechSoldier& rhs);
	virtual ~CTechSoldier() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Imgui_RenderProperty() override;

private:
	class CTechSoldierFSM*		m_pFSM = nullptr;
	_double					m_MoveTime = 0.0;

private:
	void Collision_ToPlayer();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTechSoldier* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END