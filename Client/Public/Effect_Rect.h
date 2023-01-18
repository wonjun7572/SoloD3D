#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CEffect_Rect : public CGameObject
{
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

	void	Set_Play(_bool bPlay) { m_bPlay = bPlay; }
	void	Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }

	void	Imgui_RenderProperty() override;
	void	LinkObject(_double TimeDelta, _fvector targetpos);
	void	Compute_BillBoard();

protected:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom = nullptr;
	CVIBuffer_Rect*				m_pVIBufferCom = nullptr;
	_float						m_fAlpha = 0.f;
	_bool						m_bPlay = false;
	_float2						m_UVMoveFactor;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END