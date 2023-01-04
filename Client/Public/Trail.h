#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CTrail final : public CGameObject
{
private:
	CTrail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTrail(const CTrail& rhs);
	virtual ~CTrail() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	HRESULT Trail1_Render(_float fMove, _float4x4 mat);
	HRESULT Trail2_Render(_float fMove, _float4x4 mat);
	HRESULT Trail3_Render(_float fMove, _float4x4 mat);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;

	CModel*					m_pTrail1Com = nullptr;
	
	CModel*					m_pTrail2Com = nullptr;
	
	CModel*					m_pTrail3Com = nullptr;

private:
	HRESULT SetUp_Components();

	HRESULT Trail1_SetUp_ShaderResources(_float fMove, _float4x4 mat);
	HRESULT Trail2_SetUp_ShaderResources(_float fMove, _float4x4 mat);
	HRESULT Trail3_SetUp_ShaderResources(_float fMove, _float4x4 mat);

public:
	static CTrail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END