#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CCollider;
class CRenderer;
END

BEGIN(Client)

class CIzuna final : public CGameObject
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE, COLLTYPE_END };
	enum STATETYPE	{ STATE_NORMAL, STATE_STAND, STATE_KNEEL, STATE_END };

private:
	CIzuna(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIzuna(const CIzuna& rhs);
	virtual ~CIzuna() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	CModel*					Get_ModelComponent() { return m_pModelCom; }

private:
	void	Collision_ToObstacle(_double TimeDelta);
	void	Collision_ToEnemy(_double TimeDelta);

private:
	void Imgui_RenderProperty() override;
	_float3					m_ObstaclePos;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };

private:
	_uint					m_iCurrentAnimIndex = 0;
	_bool					m_bAnimationFinished = false;

private:
	_double					m_MoveTime = 0;
	STATETYPE				m_eState = STATE_END;
	class CIzunaFSM*		m_pFSM = nullptr;

public:
	STATETYPE				Get_StateType() { return m_eState; }

	_bool					Get_IsRun() { return m_isRun; }
	_bool					Get_IsAttack() { return m_isAttack; }
	_bool					Get_IsJumping() { return m_isJumping; }
	_bool					Get_IsExSkilling() { return m_isExSkill; }

private:
	_bool					m_isAttack = false;
	_bool					m_isRun = true;
	_bool					m_isJumping = false;
	_bool					m_isExSkill = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CIzuna* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END