#include "stdafx.h"
#include "..\Public\Parts.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Player.h"

CParts::CParts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CParts::CParts(const CParts & rhs)
	:CGameObject(rhs)
{
}

HRESULT CParts::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CParts::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	m_vRimColor = _float4(0.1f, 0.1f, 0.3f, 1.f);
	m_bHasShadow = true;
	return S_OK;
}

void CParts::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CParts::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CParts::Render()
{
	return S_OK;
}

void CParts::Play_AdditiveAnim(_double TimeDelta, float fRatio)
{
	m_pModelCom[m_eModelState]->Set_AdditiveAnimIndex(m_iAdditiveAnimIndex);
	m_pModelCom[m_eModelState]->Play_AddtivieAnim(TimeDelta, fRatio);
}

void CParts::LinkPlayer(CTransform * pTarget)
{
	m_pTransformCom->SetWorldMatrix(pTarget->Get_WorldMatrix());
}

HRESULT CParts::PartsRender(_uint iPassIndex)
{
	_uint iNumMeshes = m_pModelCom[m_eModelState]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom[m_eModelState]->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		bool HasNormal = false;
		if (FAILED(m_pModelCom[m_eModelState]->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture")))
			HasNormal = false;
		else
			HasNormal = true;

		m_pShaderCom->Set_RawValue("g_HasNormal", &HasNormal, sizeof(bool));

		bool HasSpecular = false;
		if (FAILED(m_pModelCom[m_eModelState]->Bind_Material(m_pShaderCom, i, aiTextureType_SPECULAR, "g_SpecularTexture")))
			HasSpecular = false;
		else
			HasSpecular = true;

		m_pShaderCom->Set_RawValue("g_HasSpecular", &HasSpecular, sizeof(bool));
		m_pModelCom[m_eModelState]->Render(m_pShaderCom, i, iPassIndex, "g_BoneMatrices");
	}

	return S_OK;
}

HRESULT CParts::PartsShadowRender(_uint iPassIndex)
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShadowShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShadowShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
		return E_FAIL;
	if (FAILED(m_pShadowShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTPROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	_uint iNumMeshes = m_pModelCom[m_eModelState]->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom[m_eModelState]->Render(m_pShadowShaderCom, i, iPassIndex, "g_BoneMatrices");
	}

	return S_OK;
}

void CParts::Imgui_RenderProperty()
{
}

HRESULT CParts::SetUp_ShaderResources()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	_float3 vCamPos = _float3(pGameInstance->Get_CamPosition().x, pGameInstance->Get_CamPosition().y, pGameInstance->Get_CamPosition().z);
	if (_float3::Distance(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), vCamPos) > 30.f)
		m_vRimColor = _float4(0.f, 0.f, 0.f, 0.f);
	else
		m_vRimColor = _float4(0.1f, 0.1f, 0.3f, 1.f);
	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CParts::ChangeModel(MODEL iModelIndex)
{
	if (m_eModelState == iModelIndex)
		return;

	m_ePreModelState = m_eModelState;
	m_eModelState = iModelIndex;

	_uint AnimNum = m_pModelCom[m_ePreModelState]->Get_AnimationsNum();
	_uint iCurrentIndex = m_pModelCom[m_ePreModelState]->Get_CurrentAnimIndex();
	_uint iPreIndex = m_pModelCom[m_ePreModelState]->Get_PreAnimIndex();
	_uint iAdditiveAnimNum = m_pModelCom[m_ePreModelState]->Get_AdditiveAnimIndex();

	m_pModelCom[m_eModelState]->Model_IndexChange(iCurrentIndex, iPreIndex, iAdditiveAnimNum);
	
	for (_uint i = 0; i < AnimNum; ++i)
	{
		_double time = m_pModelCom[m_ePreModelState]->Get_AnimPlayTime(i);
		m_pModelCom[m_eModelState]->Model_Change(i, time);
	}
}

void CParts::Free()
{
	__super::Free();

	for (_uint i = 0; i < MODEL_END; ++i)
		Safe_Release(m_pModelCom[i]);

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pShadowShaderCom);
}
