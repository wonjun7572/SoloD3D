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

	if (m_bPlay)
	{
		m_UVMove.x += static_cast<_float>(TimeDelta) * m_MEffectDesc.fMoveSpeed;

		if (m_UVMove.x >= 1.f)
		{
			m_UVMove.x = -1.f;
			m_bPlay = false;
		}
	}
	else
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
	if (m_pShaderCom != nullptr)
	{
		m_iPassnum = m_pShaderCom->Get_NumPasses();
		ImGui::Text("PASSNUM: %d", m_iPassnum);
		ImGui::InputInt("PassIndex", &m_MEffectDesc.iPassIndex);
	}

	// 여러개의 디퓨즈 텍스쳐와 마스크 텍스쳐를 가져올수 있게 만들자
	ImGui::Begin("Texture");
	
	ImGui::RadioButton("Diffuse", &m_iTexRadioBtn, 0); ImGui::SameLine();
	ImGui::RadioButton("Mask", &m_iTexRadioBtn, 1); ImGui::SameLine();

	for (_uint i = 0; i < m_pDiffuseTexCom->Get_CntTex(); ++i)
	{
		if (ImGui::ImageButton((void*)m_pDiffuseTexCom->Get_Texture(i), ImVec2(60.f, 60.f)))
		{
			if(m_iTexRadioBtn == 0)
				m_MEffectDesc.iDiffuseTex = i;
			else if(m_iTexRadioBtn == 1)
				m_MEffectDesc.iMaskTex = i;
		}
		if (i == 0 || (i + 1) % 6)
			ImGui::SameLine();
	}
	
	ImGui::End();

	ImGui::NewLine();
	// 플레이 버튼 누르면 실행 되는 기능 추가 
	ImGui::DragFloat("ALPHA", &m_MEffectDesc.fAlpha, 0.01f, 0.f, 100.f);
	ImGui::DragFloat("UVSpeedY", &m_UVMove.y, 0.01f, -5.f, 5.f);
	
	if (ImGui::Button("Play"))
	{
		m_bPlay = true;
	}

	if (ImGui::Button("Pause"))
	{
		m_bPlay = false;
	}
	
	ImGui::DragFloat("MoveSpeed", &m_MEffectDesc.fMoveSpeed, 0.01f, 0.f, 10.f);

	if (m_bPlay)
	{
		m_UVMove.x += m_MEffectDesc.fMoveSpeed;

		if (m_UVMove.x >= 1.f)
			m_UVMove.x = -1.f;
	}
	else
		ImGui::DragFloat("UVSpeedX", &m_UVMove.x, 0.01f, -5.f, 5.f);
}

void CEffect_Mesh::LinkPlayer(CTransform * pTarget, _float4x4 mat)
{
	m_bLinking = true;
	_matrix WorldMat = XMLoadFloat4x4(&mat) * pTarget->Get_WorldMatrix();
	m_pTransformCom->SetWorldMatrix(WorldMat);
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
