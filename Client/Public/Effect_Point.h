#pragma once

#include "Client_Define.h"
#include "Effect.h"

BEGIN(Engine)
class CVIBuffer_Point_Instancing;
END

BEGIN(Client)

class Effect_Point : public CEffect
{
public:
	typedef struct tagPointEffectDesc
	{
		_float4x4   PivotMatrix;
		CTransform* pTargetTransform;
	}POINTDESC;

protected:
	Effect_Point(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	Effect_Point(const Effect_Point& rhs);
	virtual ~Effect_Point() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

protected:
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Point_Instancing*	m_pVIBufferCom = nullptr;
	_float						m_fFrame = 0.f;
	POINTDESC					m_ParticleDesc;
	_bool						m_bLinking = false;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END