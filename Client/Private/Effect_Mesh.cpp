#include "stdafx.h"
#include "..\Public\Effect_Mesh.h"
#include "GameInstance.h"

CEffect_Mesh::CEffect_Mesh(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{
}

CEffect_Mesh::CEffect_Mesh(const CEffect_Mesh & rhs)
	:CEffect(rhs)
{
}

HRESULT CEffect_Mesh::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Mesh::Init(void * pArg)
{
	if (nullptr != pArg)
	{
		memcpy(&m_MEffectDesc, pArg, sizeof(m_MEffectDesc));
	}
	else
	{
		XMStoreFloat4x4(&m_MEffectDesc.PivotMatrix, XMMatrixIdentity());
		m_MEffectDesc.fMoveSpeed = 1.f;
		m_MEffectDesc.iPassIndex = 2;
		m_MEffectDesc.iDiffuseTex = 4;
		m_MEffectDesc.iMaskTex = 14;
		m_MEffectDesc.fAlpha = 1.f;
		m_MEffectDesc.pTargetTransform = nullptr;
	}

	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	m_UVMove =	_float2(-1.f, 0.f);

	return S_OK;
}

void CEffect_Mesh::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_UVMove.x += static_cast<_float>(TimeDelta) * m_MEffectDesc.fMoveSpeed;
	if (m_UVMove.x >= 1.f)
		m_UVMove.x = -1.f;
}

void CEffect_Mesh::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CEffect_Mesh::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Mesh::Imgui_RenderProperty()
{
}

void CEffect_Mesh::Set_PivotMatrix(_float4x4 pivotMatrix)
{
	m_MEffectDesc.PivotMatrix = pivotMatrix;
}

void CEffect_Mesh::Free()
{
	__super::Free();

	if (m_bClone)
	{
		Safe_Release(m_MEffectDesc.pTargetTransform);
	}

	Safe_Release(m_pModelCom);
	Safe_Release(m_pDiffuseTexCom);
	Safe_Release(m_pMaskTexCom);
}
