#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CLoadingBackGround final : public CGameObject
{
private:
	CLoadingBackGround(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadingBackGround(const CLoadingBackGround& rhs);
	virtual ~CLoadingBackGround() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CTexture*				m_pTextureCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;

private:
	_float					m_fX, m_fY, m_fSizeX, m_fSizeY;

	_float4x4				m_ViewMatrix;
	_float4x4				m_ProjMatrix;

	_double					m_ChangeTime = 0.0;
	_uint					m_iDeffuseTexNum = 0;

private:
	HRESULT SetUp_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CLoadingBackGround* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END