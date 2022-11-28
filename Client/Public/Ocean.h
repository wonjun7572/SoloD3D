#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)

class COcean final : public CGameObject
{
private:
	COcean(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	COcean(const COcean& rhs);
	virtual ~COcean() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CTexture*					m_pHeightTexCom = nullptr;

	_uint						m_iDiffuseTexNum = 0;
	_uint						m_iNormalTexNum = 0;
	_float						m_fTimeDelta = 0.f;
	_float						m_fWaveHeight = 5.f;
	_float						m_fSpeed = 2.f;
	_float						m_fWaveFrequency = 15.f;
	_float						m_fUVSpeed = 0.15f;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static COcean* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END