#include "stdafx.h"
#include "..\Public\Shotgun.h"
#include "GameInstance.h"
#include "Bone.h"
#include "Sheila.h"

CShotgun::CShotgun(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CWeapon(pDevice, pContext)
{
}

CShotgun::CShotgun(const CShotgun & rhs)
	: CWeapon(rhs)
{
}

HRESULT CShotgun::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CShotgun::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Set_ObjectName(TEXT("Shotgun"));

	//XMLoadFloat4x4(&m_Affinemat) = XMMatrixIdentity();

	return S_OK;
}

void CShotgun::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pPlayer = pGameInstance->Find_GameObject(LEVEL_CHAP1, TEXT("Layer_Player"), TEXT("Sheila"));

	if (m_pPlayer != nullptr)
		m_pPlayerBone = static_cast<CModel*>(m_pPlayer->Find_Component(TEXT("Com_Model")))->Get_BonePtr("Weapon_RBone");

	if (m_pPlayerBone != nullptr)
	{
		_float4x4 OffsetMat = CMathUtils::Mul_Matrix(m_pPlayerBone->Get_CombindMatrix(),
			static_cast<CModel*>(m_pPlayer->Find_Component(TEXT("Com_Model")))->Get_PivotMatrix());

			_matrix Affinemat = XMMatrixAffineTransformation(XMVectorSet(0.4f, 0.4f, 0.4f, 0.f),
				XMVectorSet(0.f, 0.f, 0.f, 1.f),
				XMQuaternionRotationRollPitchYaw(-1.9f,-1.8f, 1.75f),
				XMVectorSet(0.08f, -0.015f, -0.017f, 1.f)) ;

			_matrix TransformMat =  Affinemat *
				XMLoadFloat4x4(&OffsetMat) *
				m_pPlayer->Get_TransformCom()->Get_WorldMatrix();
				
		m_pTransformCom->SetWorldMatrix(TransformMat);
	}

	if (m_pModelCom != nullptr)
	{
		m_pModelCom->Play_Animation(TimeDelta);
		m_pModelCom->Set_AnimationIndex(m_iCurrentAnimIndex);
	}

	Safe_Release(pGameInstance);
}

void CShotgun::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CShotgun::Render()
{
	if (static_cast<CSheila*>(m_pPlayer)->Get_WeaponType() == CSheila::TYPE_SHOTGUN)
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

			if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices")))
				return E_FAIL;
		}
	}
	return S_OK;
}

HRESULT CShotgun::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Shotgun"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CShotgun::SetUp_ShaderResources()
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
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;
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

CShotgun * CShotgun::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CShotgun*		pInstance = new CShotgun(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CShotgun");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CShotgun::Clone(void * pArg)
{
	CShotgun*		pInstance = new CShotgun(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CShotgun");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CShotgun::Free()
{
	__super::Free();
}
