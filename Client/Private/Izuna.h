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

private:
	void	Move(STATETYPE eType, _double TimeDelta);
	void	Collision_ToObstacle();

private:
	void Imgui_RenderProperty() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom[COLLTYPE_END] = { nullptr };

private:
	// IDLE
	_uint					m_iCurrentAnimIndex = 7;
	_bool					m_bAnimationFinished = false;
private:
	_double					m_MoveTime = 0;

	STATETYPE				m_eState = STATE_END;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CIzuna* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END