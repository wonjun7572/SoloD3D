#pragma once

#include "Monster.h"
#include "State.h"

BEGIN(Client)

class CDemon final : public CMonster
{
private:
	CDemon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDemon(const CDemon& rhs);
	virtual ~CDemon() = default;
	friend class CDemon_State;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void Imgui_RenderProperty() override;

	CModel* Get_Model() { return m_pModelCom; }

public:
	void CollisionToPlayer(_double TimeDelta);
	void CollisionToWeapon(_double TimeDelta);

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	_bool		m_bMove = true;
	_bool		m_bAttack1 = false;
	_bool		m_bAttack2 = false;

	_bool		m_bDamaged_B = false;
	_bool		m_bDamaged_F = false;

	_bool		m_bHitDown = false;

private:
	_double		m_DamagedCoolTime = 0.0;

public:
	CState*			m_pState = nullptr;
	CDemon_State*	m_pDemon_State = nullptr;

public:
	static CDemon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END