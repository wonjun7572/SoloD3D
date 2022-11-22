#include "stdafx.h"
#include "..\Public\Ocean.h"
#include "GameInstance.h"

COcean::COcean(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

COcean::COcean(const COcean & rhs)
	:CGameObject(rhs)
{
}

HRESULT COcean::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT COcean::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;
	
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Go_Left(30);
	m_pTransformCom->Go_Down(2);
	m_pTransformCom->Set_Scaled(_float3(0.25f, 1.f, 0.25f));
	return S_OK;
}

void COcean::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fTimeDelta += static_cast<float>(TimeDelta * 0.1f);
}

void COcean::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_PRIORITY, this);
}

HRESULT COcean::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

void COcean::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("For. Texture"))
	{
		ImGui::Text("TYPE_DIFFUSE : ");
		ImGui::SameLine();
		ImGui::Text(to_string(m_iDiffuseTexNum).c_str());
		ImGui::SameLine();
		ImGui::Text(" / ");
		ImGui::SameLine();
		ImGui::Text(to_string(m_pTextureCom[TYPE_DIFFUSE]->Get_AllTexsize()).c_str());

		for (size_t i = 0; i < m_pTextureCom[TYPE_DIFFUSE]->Get_AllTexsize(); ++i)
		{
			if (ImGui::ImageButton((void*)m_pTextureCom[TYPE_DIFFUSE]->Get_Texture()[i], ImVec2(60.f, 60.f)))
				m_iDiffuseTexNum = static_cast<_uint>(i);

			if (i == 0 || (i + 1) % 6)
				ImGui::SameLine();
		}
		ImGui::NewLine();

		ImGui::Text("TYPE_NORMAL : ");
		ImGui::SameLine();
		ImGui::Text(to_string(m_iNormalTexNum).c_str());
		ImGui::SameLine();
		ImGui::Text(" / ");
		ImGui::SameLine();
		ImGui::Text(to_string(m_pTextureCom[TYPE_NORMAL]->Get_AllTexsize()).c_str());

		for (size_t i = 0; i < m_pTextureCom[TYPE_NORMAL]->Get_AllTexsize(); ++i)
		{
			if (ImGui::ImageButton((void*)m_pTextureCom[TYPE_NORMAL]->Get_Texture()[i], ImVec2(60.f, 60.f)))
				m_iDiffuseTexNum = static_cast<_uint>(i);

			if (i == 0 || (i + 1) % 6)
				ImGui::SameLine();
		}
		ImGui::NewLine();
	}

	if (ImGui::CollapsingHeader("For. WaveProperty"))
	{
		ImGui::DragFloat("WaveHeight", &m_fWaveHeight, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("WaveSpeed", &m_fSpeed, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("WaveFrequency", &m_fWaveFrequency, 0.1f, 0.0f, 50.0f);
		ImGui::DragFloat("WaveUVSpeed", &m_fUVSpeed, 0.1f, 0.0f, 50.0f);
	}
}

HRESULT COcean::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxNorTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Ocean"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Water_D"), TEXT("Com_Texture_D"),
		(CComponent**)&m_pTextureCom[TYPE_DIFFUSE])))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Water_N"), TEXT("Com_Texture_N"),
		(CComponent**)&m_pTextureCom[TYPE_NORMAL])))
		return E_FAIL;

	return S_OK;
}

HRESULT COcean::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Time", &m_fTimeDelta, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WaveHeight", &m_fWaveHeight, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_Speed", &m_fSpeed, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_WaveFrequency", &m_fWaveFrequency, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVSpeed", &m_fUVSpeed, sizeof _float)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_pTransformCom->Get_World4x4())))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof _float4)))
		return E_FAIL;

	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom[TYPE_DIFFUSE]->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture",m_iDiffuseTexNum)))
		return E_FAIL;

	if (FAILED(m_pTextureCom[TYPE_NORMAL]->Bind_ShaderResource(m_pShaderCom, "g_NormalTexture",m_iNormalTexNum)))
		return E_FAIL;

	return S_OK;
}

COcean * COcean::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	COcean*		pInstance = new COcean(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : COcean");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * COcean::Clone(void * pArg)
{
	COcean*		pInstance = new COcean(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : COcean");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void COcean::Free()
{
	__super::Free();

	for(auto& pTexture : m_pTextureCom)
		Safe_Release(pTexture);

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
