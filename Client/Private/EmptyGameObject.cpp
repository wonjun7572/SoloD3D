#include "stdafx.h"
#include "..\Public\EmptyGameObject.h"
#include "GameInstance.h"
#include "GameUtils.h"

CEmptyGameObject::CEmptyGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

CEmptyGameObject::CEmptyGameObject(const CEmptyGameObject & rhs)
	:CGameObject(rhs)
{
}

HRESULT CEmptyGameObject::Init_Prototype()
{
	m_szModelTag = new _tchar[MAX_PATH];

	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEmptyGameObject::Init(void * pArg)
{
	m_szModelTag = new _tchar[MAX_PATH];

	if(pArg != nullptr)
		wcscpy_s(m_szModelTag, MAX_PATH, reinterpret_cast<_tchar*>(pArg));

	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	return S_OK;
}

void CEmptyGameObject::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CEmptyGameObject::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CEmptyGameObject::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	if (m_pModelCom != nullptr)
	{
		_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; ++i)
		{
			if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture")))
				return E_FAIL;

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i)))
				return E_FAIL;
		}
	}

	return S_OK;
}

void CEmptyGameObject::Imgui_RenderProperty()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	static const _tchar* szModelName = L"";
	pGameInstance->Imgui_ComponentViewer(LEVEL_CHAP1, szModelName);
	
	ImGui::NewLine();

	char szName[MAX_PATH] = {};
	CGameUtils::wc2c(szModelName, szName);
	ImGui::Text(szName);

	if (ImGui::Button("Add Model Component"))
	{
		/* For.Com_Model */
		__super::Add_Component(LEVEL_CHAP1, szModelName, TEXT("Com_Model"), (CComponent**)&m_pModelCom);
		m_szModelTag = new _tchar[MAX_PATH];
		wcscpy_s(m_szModelTag, MAX_PATH, szModelName);
	}
	
	RELEASE_INSTANCE(CGameInstance);
}

const _tchar * CEmptyGameObject::Get_ModelTag()
{
	return m_szModelTag;
}

HRESULT CEmptyGameObject::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEmptyGameObject::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	/* For.Lights */
	//const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	//if (nullptr == pLightDesc)
	//	return E_FAIL;
	//
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDir", &pLightDesc->vDirection, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightDiffuse", &pLightDesc->vDiffuse, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightAmbient", &pLightDesc->vAmbient, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vLightSpecular", &pLightDesc->vSpecular, sizeof(_float4))))
	//	return E_FAIL;
	//if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
	//	return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CEmptyGameObject * CEmptyGameObject::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEmptyGameObject*		pInstance = new CEmptyGameObject(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CEmptyGameObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEmptyGameObject::Clone(void * pArg)
{
	CEmptyGameObject*	pInstance = new CEmptyGameObject(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEmptyGameObject");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CEmptyGameObject::Free()
{
	__super::Free();

	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
