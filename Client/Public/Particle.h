#pragma once

#include "Client_Define.h"
#include "GameInstance.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CVIBuffer_Point_Cochlea;
END

class CParticle final : public CGameObject
{
public:
	enum PROPERTY { COCHLEA, GODIRECTION, PROPERTY_END};

	typedef struct tagEffectDesc
	{
		const _tchar* pTextureTag;
		_uint   iProperty;
		_float  fShowTime;
		_float  fSizeUV_X, fSizeUV_Y;
		_uint   iSpriteCount_X, iSpriteCount_Y;
		CVIBuffer_Point_Cochlea::DESC ViBufferDesc;
	}DESC;

private:
	CParticle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParticle(const CParticle& rhs);
	virtual ~CParticle() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void	Set_Direction(_float4 vDir) { m_vDir = vDir; }

private:
	HRESULT	SetUp_Components();
	HRESULT SetUp_ShaderResources();

private:
	CShader*								m_pShaderCom = nullptr;
	CRenderer*							m_pRendererCom = nullptr;
	CTexture*								m_pTextureCom = nullptr;
	CVIBuffer_Point_Cochlea*	m_pVIBufferCom = nullptr;

	DESC									m_Desc;
	_float4									m_vDir;

public:
	static CParticle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

