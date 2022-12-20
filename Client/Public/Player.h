#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CNavigation;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE, COLLTYPE_END };
	enum PLAYER_STATE { IDLE, ATTACK, FORWARD, LEFT, RIGHT, BACK ,LF, RF, LB, RB, STATE_END };

private:
	CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayer(const CPlayer& rhs);
	virtual ~CPlayer() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	CModel*					Get_ModelCom() { return m_pModelCom; }
	CNavigation*			Get_NaviCom() { return m_pNavigationCom; }

	void	Movement(_double TimeDelta);

	void	Set_CamTurn(_bool isTurn) { m_bCamTurn = false; }

private:
	void Imgui_RenderProperty() override;

private:
	class CPlayerFSM*		m_pFSM = nullptr;
	CModel*					m_pModelCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };
	CNavigation*			m_pNavigationCom = nullptr;

public:
	vector<CGameObject*>	Get_PlayerParts() { return m_PlayerParts; }

private:
	vector<CGameObject*>	m_PlayerParts;
	_uint					m_PartSize = 0;

	_uint					m_iCurrentAnimIndex = 23;

	_float					m_MouseSensity = 0.1f;
public:
	_bool					IsWalking() { return m_bWalk; }
	_bool					IsRunning() { return m_bRunning; }
	_bool					IsAttack_0() { return m_bAttack_0; }
	_bool					IsAttack_1() { return m_bAttack_1; }
	_bool					IsAttack_2() { return m_bAttack_2; }
	_bool					IsJumping() { return m_bJumping; }

	void					Set_State(PLAYER_STATE eState) { m_eState = eState; }
	PLAYER_STATE			Get_State() { return m_eState; }

	_bool					Get_CamTurn() { return m_bCamTurn; }

private:
	_bool					m_bCamTurn = false;
	_bool					m_bAttacking = false;
	_bool					m_bAttackClick = false;

	_bool					m_bAttack_0 = false;
	_bool					m_bAttack_1 = false;
	_bool					m_bAttack_2 = false;
	_bool					m_bAttack_3 = false;

	_double					m_AttackTime_0 = 0.0;

	_bool					m_bWalk = false;
	_bool					m_bRunning = false;

	_bool					m_bJumping = false;

	PLAYER_STATE			m_eState = STATE_END;

private:
	_bool					m_bAnimation = true;

private:
	HRESULT SetUp_Parts();
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END