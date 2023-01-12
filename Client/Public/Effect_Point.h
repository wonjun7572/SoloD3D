#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Point_Instancing;
END

BEGIN(Client)

class Effect_Point final : public CGameObject
{
private:
	Effect_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Effect_Point(const Effect_Point& rhs);
	virtual ~Effect_Point() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Point_Instancing*	m_pVIBufferCom = nullptr;

	_int						m_iNumber = 0;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static Effect_Point* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END