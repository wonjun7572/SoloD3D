#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Sphere;
class CTexture;
END

BEGIN(Client)

class CTestSphere final : public CGameObject
{
private:
	CTestSphere(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestSphere(const CTestSphere& rhs);
	virtual ~CTestSphere() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Sphere*		m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

private:
	HRESULT		SetUp_Components();
	HRESULT		SetUp_ShaderResources();

	_double		m_TimeDelta = 0;

public:
	static CTestSphere* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END