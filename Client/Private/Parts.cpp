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
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom[m_eModelState]->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");
		
		bool HasSpecular = false;
		if (FAILED(m_pModelCom[m_eModelState]->Bind_Material(m_pShaderCom, i, aiTextureType_SPECULAR, "g_SpecularTexture")))
			HasSpecular = false;
		else
			HasSpecular = true;
		
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4));
		RELEASE_INSTANCE(CGameInstance);

		m_pModelCom[m_eModelState]->Render(m_pShaderCom, i, iPassIndex, "g_BoneMatrices");
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

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CParts::SetUp_SecondShaderResources(_float4x4 matrix)
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &matrix)))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
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
}
