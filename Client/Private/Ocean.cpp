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
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;
	
	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(10.f, 1.f, 10.f));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.f, -5.f, 0.f, 1.f));

	return S_OK;
}

void COcean::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fTimeDelta += static_cast<float>(TimeDelta * m_fWaveTime);
}

void COcean::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT COcean::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 1)))
			return E_FAIL;
	}
	
	return S_OK;
}

void COcean::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("For. Wave"))
	{
		ImGui::DragFloat("WaveTime", &m_fWaveTime, 0.01f, 0.f, 50.f);
		ImGui::DragFloat("WaveHeight", &m_fWaveHeight, 0.01f, 0.f, 50.f);
		ImGui::DragFloat("WaveSpeed", &m_fSpeed, 0.01f, 0.f, 50.f);
		ImGui::DragFloat("WaveFrequency", &m_fWaveFrequency, 0.01f, 0.f, 50.f);
		ImGui::DragFloat("UVSpeed", &m_fUVSpeed, 0.01f, 0.f, 50.f);
	}
}

HRESULT COcean::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Ocean"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_HeightTexture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Ocean_Height"), TEXT("Com_HeightTexture"),
		(CComponent**)&m_pHeightTexCom)))
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

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewInverseMatrix", &pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjInverseMatrix", &pGameInstance->Get_TransformMatrix_Inverse(CPipeLine::D3DTS_PROJ))))
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

	if (FAILED(m_pHeightTexCom->Bind_ShaderResource(m_pShaderCom, "g_HeightTexture", 0)))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

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

	Safe_Release(m_pHeightTexCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
