#pragma once

#include "Effect.h"

BEGIN(Client)
class CEffect_Mesh : public CEffect
{
public:
	typedef struct tagMeshEffectDesc
	{
		_float4x4 PivotMatrix;
		_float fMoveSpeed;
		_int iPassIndex;
		_int iDiffuseTex;
		_int iMaskTex;
		_float fAlpha;
		CTransform* pTargetTransform;
	}EFFECTDESC;

protected:
	CEffect_Mesh(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect_Mesh(const CEffect_Mesh& rhs);
	virtual	~CEffect_Mesh() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();
	virtual void Imgui_RenderProperty();

	void Set_Link(_bool bLink) { m_bLinking = bLink; }
	void Set_UV(_float2 UV) { m_UVMove = UV; }
	void Set_Alpha(_float fAlpha) { m_MEffectDesc.fAlpha = fAlpha; }
	void Set_MoveSpeed(_float fSpeed) { m_MEffectDesc.fMoveSpeed = fSpeed; }
	void Set_PivotMatrix(_float4x4 pivotMatrix);

protected:
	EFFECTDESC	m_MEffectDesc;
	CModel*		m_pModelCom = nullptr;
	CTexture*   m_pDiffuseTexCom = nullptr;
	CTexture*   m_pMaskTexCom = nullptr;
	_float2		m_UVMove;
	_int        m_iTexRadioBtn = 0;
	_int		m_iPassnum = 0;
	_bool		m_bLinking = false;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END