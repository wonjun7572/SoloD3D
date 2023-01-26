#pragma once

#include "Effect.h"

BEGIN(Client)

class CEffect_Mesh : public CEffect
{
public:
	typedef struct tagMeshEffectDesc
	{
		_float4x4   PivotMatrix;
		_int		iPassIndex;
		_int		iDiffuseTex;
		_int		iMaskTex;
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

	void Set_PivotMatrix(_float4x4 pivotMatrix);
	void Set_Link(_bool bLink) { m_bLinking = bLink; }
	void Set_Target(CTransform* pTarget);

protected:
	EFFECTDESC	m_MEffectDesc;
	
	CModel*		m_pModelCom = nullptr;
	CTexture*   m_pDiffuseTexCom = nullptr;
	CTexture*   m_pMaskTexCom = nullptr;
	_bool		m_bLinking = false;

	_int        m_iTexRadioBtn = 0;
	_int		m_iPassnum = 0;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END