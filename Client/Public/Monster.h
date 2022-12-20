#pragma once

#include "Client_Define.h"
#include "GameObject.h"
#include "Player.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CCollider;
END

BEGIN(Client)

class CMonster abstract : public CGameObject
{
public:
	enum COLLIDERTYPE { COLLTYPE_AABB, COLLTYPE_OBB, COLLTYPE_SPHERE, COLLTYPE_END };

protected:
	CMonster(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMonster(const CMonster& rhs);
	virtual ~CMonster() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	CModel* Get_ModelCom() { return m_pModelCom; }

protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };
	_bool					m_bAnimation = false;
	_uint					m_iCurrentAnimIndex = 0;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END