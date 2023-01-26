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
		m_MEffectDesc.iPassIndex = 2;
		m_MEffectDesc.iDiffuseTex = 4;
		m_MEffectDesc.iMaskTex = 14;
		m_MEffectDesc.pTargetTransform = nullptr;
	}

	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	m_UVMoveFactor = _float2(-1.f, 0.f);

	return S_OK;
}

void CEffect_Mesh::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
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
	ImGui::DragFloat("UV", &m_fUVSpeed, 0.1f, -10.f, 10.f);
	ImGui::DragFloat("Alpha", &m_fAlpha, 0.1f, -10.f, 10.f);
	ImGui::DragFloat("MoveX", &m_UVMoveFactor.x, 0.1f, -10.f, 10.f);
	ImGui::DragFloat("MoveY", &m_UVMoveFactor.y, 0.1f, -10.f, 10.f);

	if (m_pShaderCom != nullptr)
	{
		m_iPassnum = m_pShaderCom->Get_NumPasses();
		ImGui::Text("PASSNUM: %d", m_iPassnum);
		ImGui::InputInt("PassIndex", &m_MEffectDesc.iPassIndex);
	}

	ImGui::Begin("Texture");

	ImGui::RadioButton("Diffuse", &m_iTexRadioBtn, 0); ImGui::SameLine();
	ImGui::RadioButton("Mask", &m_iTexRadioBtn, 1); ImGui::SameLine();

	for (_uint i = 0; i < m_pDiffuseTexCom->Get_CntTex(); ++i)
	{
		if (ImGui::ImageButton((void*)m_pDiffuseTexCom->Get_Texture(i), ImVec2(60.f, 60.f)))
		{
			if (m_iTexRadioBtn == 0)
				m_MEffectDesc.iDiffuseTex = i;
			else if (m_iTexRadioBtn == 1)
				m_MEffectDesc.iMaskTex = i;
		}
		if (i == 0 || (i + 1) % 6)
			ImGui::SameLine();
	}

	ImGui::End();
}

void CEffect_Mesh::Set_PivotMatrix(_float4x4 pivotMatrix)
{
	m_MEffectDesc.PivotMatrix = pivotMatrix;
}

void CEffect_Mesh::Set_Target(CTransform * pTarget)
{
	Safe_Release(m_MEffectDesc.pTargetTransform);
	m_MEffectDesc.pTargetTransform = pTarget;
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
