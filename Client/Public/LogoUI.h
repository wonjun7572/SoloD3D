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

class CLogoUI final : public CGameObject
{
private:
	CLogoUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLogoUI(const CLogoUI& rhs);
	virtual ~CLogoUI() = default;

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

private:
	HRESULT SetUp_Components();
	HRESULT	SetUp_ShaderResources();

public:
	static CLogoUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END