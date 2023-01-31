#pragma once

#include "Client_Define.h"
#include "Effect.h"

BEGIN(Engine)
class CVIBuffer_Trail;
END

BEGIN(Client)

class CEffect_Trail : public CEffect
{
protected:
	CEffect_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Trail(const CEffect_Trail& rhs);
	virtual ~CEffect_Trail() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void	Imgui_RenderProperty() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

protected:
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Trail*			m_pVIBufferCom = nullptr;
	VTXNORTEX*					m_pTrailVtx = nullptr;
	_int						m_iCntX = 0;
	_int						m_iCntZ = 0;
	_int						m_iLocalCoordinates = 0;
	_float4						m_vColor;

private:
	_float4x4					m_pTargetMatrix;
	_float3						m_vStart;
	_float3						m_vEnd;

public:
	static CEffect_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
