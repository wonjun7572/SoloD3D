#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
class CNavigation;
class CAnimation;
END

BEGIN(Client)

class CPlayer final : public CGameObject
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE, COLLTYPE_END };
	
	enum PLAYER_STATE 
	{
		PLAYER_READY, PLAYER_FM, PLAYER_BM, PLAYER_RM, PLAYER_LM
		, PLAYER_FR, PLAYER_FL, PLAYER_BR, PLAYER_BL, PLAYER_FOR
		, PLAYER_WALKF,PLAYER_WALKB
		, PLAYER_GETUP, PLAYER_DOWN, PLAYER_PASSOUT
		, PLAYER_ATTACK1, PLAYER_ATTACK2, PLAYER_ATTACK3
		, PLAYER_SK01, PLAYER_SK02, PLAYER_SK03
		, PLAYER_DMG_F , STATE_END
	};

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

	void	Set_AnimIndex(PLAYER_STATE eType);

	void	MoveToAnim(_double TimeDelta);

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
	_float					m_MouseSensity = 0.1f;

public:
	_bool					IsRunning() { return m_bRunning; }

	_bool					IsWalking() { return m_bWalk; }

	_bool					IsAttack() { return m_bAttack; }

	_bool					IsJumping() { return m_bJumping; }

	void					Set_State(PLAYER_STATE eState) { m_eState = eState; }
	PLAYER_STATE			Get_State() { return m_eState; }

	_bool					Get_CamTurn() { return m_bCamTurn; }

private:
	_bool					m_bMove = true;
	_bool					m_bAction = false;

	_bool					m_bCamTurn = false;

	_bool					m_bAttack = false;

	_bool					m_bWalk = false;
	_bool					m_bRunning = false;
	_bool					m_bJumping = false;

	_float					m_fVelocity = 1.f;

	PLAYER_STATE			m_eState = PLAYER_READY;

	_float4					m_vAnimationMove = _float4(0.f, 0.f, 0.f, 1.f);

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