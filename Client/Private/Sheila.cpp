#include "stdafx.h"
#include "..\Public\Sheila.h"
#include "GameInstance.h"
#include "Weapon.h"

CSheila::CSheila(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

CSheila::CSheila(const CSheila & rhs)
	:CGameObject(rhs)
{
}

HRESULT CSheila::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSheila::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	
	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	Set_ObjectName(TEXT("Sheila"));

	return S_OK;
}

void CSheila::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	FindWeapon();
	SwitchWeapon();
	Movement(TimeDelta);

	if (m_pModelCom != nullptr)
	{
		m_pModelCom->Play_Animation(TimeDelta);
		m_pModelCom->Set_AnimationIndex(m_iCurrentAnimIndex);
	}
}

void CSheila::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CSheila::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices")))
			return E_FAIL;
	}
		
	return S_OK;
}

void CSheila::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("For.Animation"))
	{
		const char* combo_preview_value = m_pModelCom->Get_AnimationName()[m_iCurrentAnimIndex];

		if (ImGui::BeginCombo("ANIM", combo_preview_value))
		{
			for (_uint i = 0; i < m_pModelCom->Get_AnimationsNum(); i++)
			{
				const bool is_selected = (m_iCurrentAnimIndex == i);
				if (ImGui::Selectable(m_pModelCom->Get_AnimationName()[i], is_selected))
					m_iCurrentAnimIndex = i;

				if (is_selected)
					ImGui::SetItemDefaultFocus();
			}
			ImGui::EndCombo();
		}

		ImGui::Text("Current Anim Index"); ImGui::SameLine();
		ImGui::Text(to_string(m_iCurrentAnimIndex).c_str());
	}

	ImGui::InputFloat("Sensitivity", &m_fSensitivity);
}

void CSheila::FindWeapon()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if(m_WeaponArray[TYPE_RIFLE] == nullptr)
		m_WeaponArray[TYPE_RIFLE] =	static_cast<CWeapon*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, TEXT("Layer_Weapon"), TEXT("Rifle")));
	
	if(m_WeaponArray[TYPE_SHOTGUN] == nullptr)
		m_WeaponArray[TYPE_SHOTGUN] = static_cast<CWeapon*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, TEXT("Layer_Weapon"), TEXT("Shotgun")));
	
	if(m_WeaponArray[TYPE_PISTOL] == nullptr)
		m_WeaponArray[TYPE_PISTOL] = static_cast<CWeapon*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, TEXT("Layer_Weapon"), TEXT("Pistol")));

	if(m_WeaponArray[TYPE_SNIPER] == nullptr)
		m_WeaponArray[TYPE_SNIPER] = static_cast<CWeapon*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, TEXT("Layer_Weapon"), TEXT("Sniper")));

	RELEASE_INSTANCE(CGameInstance);
}

void CSheila::SwitchWeapon()
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Key_Down(DIK_1))
		m_WeaponType = TYPE_RIFLE;
	else	if (pGameInstance->Key_Down(DIK_2))
		m_WeaponType = TYPE_SHOTGUN;
	else if (pGameInstance->Key_Down(DIK_3))
		m_WeaponType  =  TYPE_PISTOL;
	else if (pGameInstance->Key_Down(DIK_4))
		m_WeaponType = TYPE_SNIPER;

	RELEASE_INSTANCE(CGameInstance);
}

void CSheila::Movement(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	// 무기 체인지 할때 put up put down~

	switch (m_WeaponType)
	{
	case TYPE_RIFLE:
	{
		// 82 ~ 97 라이플
		if (pGameInstance->Get_DIKeyState(DIK_W))
		{
			m_pTransformCom->Go_Straight(TimeDelta);
			m_iCurrentAnimIndex = 97;
		}

		if (pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_pTransformCom->Go_Backward(TimeDelta);
			m_iCurrentAnimIndex = 97;
		}

		if (pGameInstance->Get_DIKeyState(DIK_A))
		{
			m_pTransformCom->Go_Left(TimeDelta);
			m_iCurrentAnimIndex = 97;
		}

		if (pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_pTransformCom->Go_Right(TimeDelta);
			m_iCurrentAnimIndex = 97;
		}

		if (pGameInstance->Get_DIKeyState(DIK_R))
		{
			m_iCurrentAnimIndex = 90;
		}

		_long			MouseMove = 0;

		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * m_fSensitivity);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * m_fSensitivity);
		}

		if (pGameInstance->Get_DIMouseState(DIM_LB))
		{
			m_iCurrentAnimIndex = 82;
		}
	}
		break;
	case TYPE_SHOTGUN:
	{
		if (pGameInstance->Get_DIKeyState(DIK_W))
		{
			m_pTransformCom->Go_Straight(TimeDelta);
			m_iCurrentAnimIndex = 38;
		}

		if (pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_pTransformCom->Go_Backward(TimeDelta);
			m_iCurrentAnimIndex = 38;
		}

		if (pGameInstance->Get_DIKeyState(DIK_A))
		{
			m_pTransformCom->Go_Left(TimeDelta);
			m_iCurrentAnimIndex = 38;
		}

		if (pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_pTransformCom->Go_Right(TimeDelta);
			m_iCurrentAnimIndex = 38;
		}

		if (pGameInstance->Get_DIKeyState(DIK_R))
		{
			m_iCurrentAnimIndex = 35;
		}

		_long			MouseMove = 0;

		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * m_fSensitivity);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * m_fSensitivity);
		}

		if (pGameInstance->Get_DIMouseState(DIM_LB))
		{
			m_iCurrentAnimIndex = 28;
		}
	}
	break;
	case TYPE_PISTOL:
	{
		if (pGameInstance->Get_DIKeyState(DIK_W))
		{
			m_pTransformCom->Go_Straight(TimeDelta);
			m_iCurrentAnimIndex = 77;
		}

		if (pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_pTransformCom->Go_Backward(TimeDelta);
			m_iCurrentAnimIndex = 77;
		}

		if (pGameInstance->Get_DIKeyState(DIK_A))
		{
			m_pTransformCom->Go_Left(TimeDelta);
			m_iCurrentAnimIndex = 77;
		}

		if (pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_pTransformCom->Go_Right(TimeDelta);
			m_iCurrentAnimIndex = 77;
		}

		if (pGameInstance->Get_DIKeyState(DIK_R))
		{
			m_iCurrentAnimIndex = 71;
		}

		_long			MouseMove = 0;

		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * m_fSensitivity);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * m_fSensitivity);
		}

		if (pGameInstance->Get_DIMouseState(DIM_LB))
		{
			m_iCurrentAnimIndex = 63;
		}
	}
	break;
	case TYPE_SNIPER:
	{
		if (pGameInstance->Get_DIKeyState(DIK_W))
		{
			m_pTransformCom->Go_Straight(TimeDelta);
			m_iCurrentAnimIndex = 58;
		}

		if (pGameInstance->Get_DIKeyState(DIK_S))
		{
			m_pTransformCom->Go_Backward(TimeDelta);
			m_iCurrentAnimIndex = 58;
		}

		if (pGameInstance->Get_DIKeyState(DIK_A))
		{
			m_pTransformCom->Go_Left(TimeDelta);
			m_iCurrentAnimIndex = 58;
		}

		if (pGameInstance->Get_DIKeyState(DIK_D))
		{
			m_pTransformCom->Go_Right(TimeDelta);
			m_iCurrentAnimIndex = 58;
		}

		if (pGameInstance->Get_DIKeyState(DIK_R))
		{
			m_iCurrentAnimIndex = 55;
		}

		_long			MouseMove = 0;

		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_X))
		{
			m_pTransformCom->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), TimeDelta * MouseMove * m_fSensitivity);
		}

		if (MouseMove = pGameInstance->Get_DIMouseMove(DIMS_Y))
		{
			m_pTransformCom->Turn(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), TimeDelta * MouseMove * m_fSensitivity);
		}

		if (pGameInstance->Get_DIMouseState(DIM_LB))
		{
			m_iCurrentAnimIndex = 46;
		}
	}
		break;
	default:
		break;
	}
	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CSheila::SetUp_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Sheila"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSheila::SetUp_ShaderResources()
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

CSheila * CSheila::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSheila*		pInstance = new CSheila(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CSheila");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CSheila::Clone(void * pArg)
{
	CSheila*		pInstance = new CSheila(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CSheila");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CSheila::Free()
{
	__super::Free();

	Safe_Release(m_pFSMCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
