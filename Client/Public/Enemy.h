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

class CEnemy : public CGameObject
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE, COLLTYPE_END };

protected:
	CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEnemy(const CEnemy& rhs);
	virtual ~CEnemy() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void Imgui_RenderProperty() override;

public:
	CModel*					Get_ModelComponent() { return m_pModelCom; }
	_bool					Get_IsRun() { return m_isRun; }
	_bool					Get_IsAttack() { return m_isAttack; }
	_bool					Get_IsDeath() { return m_isDeath; }

protected:
	_bool					m_bAnimationFinished = false;
	_uint					m_iCurrentAnimIndex = 0;

protected:
	_bool					m_isRun = false;
	_bool					m_isAttack = false;
	_bool					m_isDeath = false;

protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END