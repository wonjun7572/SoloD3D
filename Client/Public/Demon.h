#pragma once

#include "Monster.h"

BEGIN(Client)

class CDemon final : public CMonster
{
public:
	enum DEMON_STATE { PLAYER_BACK, PLAYER_FRONT , STATE_END};

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
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	_bool	IsWalking() { return m_bWalk; }
	_bool	IsRunning() { return m_bRun; }
	_bool	IsHitting() { return m_bHit; }
	_bool   IsAttacking() { return m_bAttack; }
	void	Set_IsKnockDowning(_bool isKD) { m_bKnockDown = isKD; }
	_bool	IsKnockDowning() { return m_bKnockDown; }

	DEMON_STATE Get_State() { return m_eState; }

private:
	class CDemonFSM* m_pFSM = nullptr;
	_bool m_bHit = false;
	_bool m_bAttack = false;
	_bool m_bRun = false;
	_bool m_bWalk = false;
	_bool m_bKnockDown = false;

	DEMON_STATE m_eState = STATE_END;

public:
	static CDemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END