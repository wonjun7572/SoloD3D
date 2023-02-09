#pragma once

#include "Client_Define.h"
#include "Effect.h"

BEGIN(Engine)
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Rect : public CEffect
{
public:
	typedef struct tagRectEffectDesc
	{
		CTransform* pTargetTransform;
		_float4x4	PivotMatrix;
	}RECTEFFECTDESC;

protected:
	CEffect_Rect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Rect(const CEffect_Rect& rhs);
	virtual ~CEffect_Rect() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void	Set_Linking(_bool bLink) { m_bLinking = bLink; }
	void	Set_PivotMatrix(_fmatrix mat) { XMStoreFloat4x4(&m_RectEffectDesc.PivotMatrix, mat); }
	void	Imgui_RenderProperty() override;
	void	Compute_BillBoard();

	void	Set_Size(_float fSize) { m_CSize[0] = fSize; m_CSize[1] = fSize; }

protected:
	RECTEFFECTDESC		m_RectEffectDesc;
	CTexture*						m_pTextureCom = nullptr;
	CVIBuffer_Rect*			m_pVIBufferCom = nullptr;
	_bool							m_bLinking = false;

	_float							 m_CSize[3] = { 1.f, 1.f , 1.f };

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END