#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
class CCollider;
END

BEGIN(Client)

class CMeteor final : public CGameObject
{
public:
	typedef struct MeteorDesc
	{
		_float4 vDestination; // 얘가 먼저 생겨 그리고 돌은 따라와.
	}DESC;

public:
	CMeteor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMeteor(const CMeteor& rhs);
	virtual ~CMeteor() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual	HRESULT	Init(void* pArg) override;
	virtual	void	Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;
	virtual void Imgui_RenderProperty() override;

	void		CollisionToAttack(_double TimeDelta);

private:
	CModel*				m_pModelCom       = nullptr;
	CShader*				m_pShaderCom     = nullptr;
	CRenderer*			m_pRendererCom  = nullptr;
	CCollider*				m_pColliderCom	  = nullptr;
	CTexture*				m_pDissolveTexCom = nullptr;

	CGameObject*				m_pParticle = nullptr;
	CGameObject*				m_pAfterParticle = nullptr;
	CGameObject*				m_pTargetUI = nullptr;

	class CPlayer*				m_pPlayer = nullptr;
	class CPlayerCamera*	m_pCam = nullptr;
	_float							m_fDissolveAmount = -1.f;
	_float							m_fFringeAmount = 1.f;

	_bool							m_bDestination = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT	SetUP_ShadowShaderResources();

	DESC		m_Desc;

public:
	static CMeteor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END