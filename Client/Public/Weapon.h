#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CBone;
class CCollider;
END

BEGIN(Client)

class CWeapon final : public CGameObject
{
public:
	typedef struct tagWeaponDesc
	{
		_float4x4			PivotMatrix;
		CBone*				pSocket;
		CTransform*			pTargetTransform;
	}WEAPONDESC;

private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	CCollider*				Get_Collider() { return m_pColliderCom; }

	void Set_TargetTrasform(const WEAPONDESC& WeaponDesc);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom =  nullptr;
	
private:
	WEAPONDESC				m_WeaponDesc;
	_float4x4				m_SocketMatrix;

	_float m_X = 0.f, m_Y = 0.f, m_Z = 0.f;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	void Imgui_RenderProperty() override;

public:
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END