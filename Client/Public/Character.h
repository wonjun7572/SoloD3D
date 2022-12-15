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

class CCharacter : public CGameObject
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE, COLLTYPE_END };
	enum STATETYPE { STATE_NORMAL, STATE_STAND, STATE_KNEEL, STATE_END };

public:
	CCharacter(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCharacter(const CCharacter& rhs);
	virtual ~CCharacter() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	CModel*		Get_ModelComponent() { return m_pModelCom; }

protected:
	void Imgui_RenderProperty() override;

protected:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CCollider*					m_pColliderCom[COLLTYPE_END] = { nullptr };

	class CCamera_Dynamic*			m_pCam = nullptr;

protected:
	// IDLE
	_uint						m_iCurrentAnimIndex = 0;
	_bool						m_bAnimationFinished = false;

protected:
	STATETYPE					m_eState = STATE_END;

public:
	STATETYPE				Get_StateType() { return m_eState; }
	_bool					Get_IsRun() { return m_isRun; }
	_bool					Get_IsAttack() { return m_isAttack; }
	_bool					Get_IsJumping() { return m_isJumping; }
	_bool					Get_IsExSkilling() { return m_isExSkill; }

protected:
	_bool					m_isAttack = false;
	_bool					m_isRun = false;
	_bool					m_isJumping = false;
	_bool					m_isExSkill = false;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END