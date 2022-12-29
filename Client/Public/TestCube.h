#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CVIBuffer_Cube;
class CTexture;
END

BEGIN(Client)

class CTestCube final : public CGameObject
{
private:
	CTestCube(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTestCube(const CTestCube& rhs);
	virtual ~CTestCube() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	
	_bool	Peeking();

	void	Imgui_Transform(OUT _float3* vPos);

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Cube*			m_pVIBufferCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;

	double					m_fTimeDelta = 0;

	_float					m_Y = 0.f;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTestCube* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END