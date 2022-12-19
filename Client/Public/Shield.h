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

class CShield : public CGameObject
{
public:
	typedef struct tagShieldDesc
	{
		_float4x4			PivotMatrix;
		CBone*				pSocket;
		CTransform*			pTargetTransform;
	}SHEILDDESC;

private:
	CShield(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShield(const CShield& rhs);
	virtual ~CShield() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CCollider*				m_pColliderCom = nullptr;

private:
	SHEILDDESC				m_SheildDesc;
	_float4x4				m_SocketMatrix;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	void Imgui_RenderProperty() override;

public:
	static CShield* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END