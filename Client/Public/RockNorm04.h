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

class CRockNorm04 final : public CGameObject
{
public:

	enum RockProperty { DEMON, FLOGAS , DEMON_SKILL4, ROCK_END };

	typedef struct RockMadaDesc
	{
		_uint iProperty;
		_float4 vDestination; 
		_float4 vDir;
	}DESC;

public:
	CRockNorm04(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRockNorm04(const CRockNorm04& rhs);
	virtual ~CRockNorm04() = default;

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
	CModel*				m_pModelCom = nullptr;
	CShader*				m_pShaderCom = nullptr;
	CRenderer*			m_pRendererCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;
	CTexture*				m_pDissolveTexCom = nullptr;

	CGameObject*				m_pParticle = nullptr;

	class CPlayer*				m_pPlayer = nullptr;
	_float							m_fDissolveAmount = -1.f;
	_float							m_fFringeAmount = 1.f;

	_double						m_dTimeDelta = 0.0;

	_float4							m_vForFlogasDir;
	_bool							m_bShoot = false;


private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT	SetUP_ShadowShaderResources();

	DESC		m_Desc;

public:
	static CRockNorm04* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END