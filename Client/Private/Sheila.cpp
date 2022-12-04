#include "stdafx.h"
#include "..\Public\Sheila.h"
#include "GameInstance.h"

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

	m_pFSMCom->Tick(TimeDelta);

	if (m_pModelCom != nullptr)
		m_pModelCom->Play_Animation(TimeDelta);
	
	CGameInstance* pinst = GET_INSTANCE(CGameInstance);

	if (pinst->Key_Down(DIK_UPARROW))
		m_iCurrentAnimIndex++;
	
	if(pinst->Key_Down(DIK_DOWNARROW))
		m_iCurrentAnimIndex--;
	
	m_pModelCom->Set_AnimationIndex(m_iCurrentAnimIndex);

	RELEASE_INSTANCE(CGameInstance);
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
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture")))
			return E_FAIL;

		if (FAILED(m_pModelCom->Render(m_pShaderCom, i, 0, "g_BoneMatrices")))
			return E_FAIL;
	}
		
	return S_OK;
}

void CSheila::Idle_OnStart()
{
	// 아이들로 돌아가는 애니메이션?
}

void CSheila::Idle_Tick(_double TimeDelta)
{
	// 아이들 애니메이션
}

void CSheila::Idle_OnExit()
{
	// 아이들에서 워크로
}

bool CSheila::IdleToWalk()
{
	CGameInstance* pInst = GET_INSTANCE(CGameInstance);

	if (pInst->Get_DIKeyState(DIK_W))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	if (pInst->Get_DIKeyState(DIK_A))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	if (pInst->Get_DIKeyState(DIK_S))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}
	if (pInst->Get_DIKeyState(DIK_D))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	RELEASE_INSTANCE(CGameInstance);
	return false;
}

bool CSheila::WalkToRun()
{
	CGameInstance* pInst = GET_INSTANCE(CGameInstance);

	if (pInst->Get_DIKeyState(DIK_W) && pInst->Get_DIKeyState(DIK_LSHIFT))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	RELEASE_INSTANCE(CGameInstance);
	return false;
}

void CSheila::Walk_OnStart()
{

}

void CSheila::Walk_Tick(_double TimeDelta)
{
	CGameInstance* pInst = GET_INSTANCE(CGameInstance);

	if (pInst->Get_DIKeyState(DIK_W))
		m_pTransformCom->Go_Straight(TimeDelta);
	if (pInst->Get_DIKeyState(DIK_A))
		m_pTransformCom->Go_Left(TimeDelta);
	if (pInst->Get_DIKeyState(DIK_S))
		m_pTransformCom->Go_Backward(TimeDelta);
	if (pInst->Get_DIKeyState(DIK_D))
		m_pTransformCom->Go_Right(TimeDelta);

	RELEASE_INSTANCE(CGameInstance);
}

void CSheila::Walk_OnExit()
{

}

bool CSheila::WalkToIdle()
{
	if (IdleToWalk() == false)
		return true;

	return false;
}

void CSheila::Run_OnStart()
{
}

void CSheila::Run_Tick(_double TimeDelta)
{
	m_pTransformCom->Go_Straight(TimeDelta * 2);
}

void CSheila::Run_OnExit()
{
}

bool CSheila::RunToIdle()
{
	if (WalkToRun() == false && WalkToIdle() == true)
		return true;

	return false;
}

void CSheila::Dash_OnStart()
{
	CGameInstance* pInst = GET_INSTANCE(CGameInstance);

	if (pInst->Get_DIKeyState(DIK_A) && pInst->Get_DIKeyState(DIK_LSHIFT))
		m_pTransformCom->Go_Left(5);
	if (pInst->Get_DIKeyState(DIK_S) && pInst->Get_DIKeyState(DIK_LSHIFT))
		m_pTransformCom->Go_Backward(5);
	if (pInst->Get_DIKeyState(DIK_D) && pInst->Get_DIKeyState(DIK_LSHIFT))
		m_pTransformCom->Go_Right(5);

	RELEASE_INSTANCE(CGameInstance);
}

void CSheila::Dash_Tick(_double TimeDelta)
{
}

void CSheila::Dash_OnExit()
{
}

bool CSheila::DashToWalk()
{
	return true;
}

bool CSheila::WalkToDash()
{
	CGameInstance* pInst = GET_INSTANCE(CGameInstance);

	if (pInst->Get_DIKeyState(DIK_A) && pInst->Get_DIKeyState(DIK_LSHIFT))
		return true;
	if (pInst->Get_DIKeyState(DIK_S) && pInst->Get_DIKeyState(DIK_LSHIFT))
		return true;
	if (pInst->Get_DIKeyState(DIK_D) && pInst->Get_DIKeyState(DIK_LSHIFT))
		return true;

	RELEASE_INSTANCE(CGameInstance);
	return false;
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
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Fiona"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	CFSMComponentBuilder builder = CFSMComponentBuilder()
		.InitState(TEXT("Idle"))						
		.AddState(TEXT("Idle"))							
			.OnStart(this, &CSheila::Idle_OnStart)	      
			.Tick(this, &CSheila::Idle_Tick)		      
			.OnExit(this, &CSheila::Idle_OnExit)          
			.Transition(TEXT("Walk"), FSM_TRANSITION(TEXT("Idle To Walk"), this, &CSheila::IdleToWalk))
		.AddState(TEXT("Walk")) 
			.OnStart(this, &CSheila::Walk_OnStart)
			.Tick(this, &CSheila::Walk_Tick)
			.OnExit(this, &CSheila::Walk_OnExit)
			.Transition(TEXT("Dash"), FSM_TRANSITION(TEXT("Walk To Dash"), this, &CSheila::WalkToDash))
			.Transition(TEXT("Run"), FSM_TRANSITION(TEXT("Walk To Run"), this, &CSheila::WalkToRun))
			.Transition(TEXT("Idle"), FSM_TRANSITION(L"Walk To Idle", this, &CSheila::WalkToIdle))
		.AddState(TEXT("Run"))
			.OnStart(this, &CSheila::Run_OnStart)
			.Tick(this, &CSheila::Run_Tick)
			.OnExit(this, &CSheila::Run_OnExit)
			.Transition(TEXT("Idle"), FSM_TRANSITION(L"Run To Idle", this, &CSheila::RunToIdle))
		.AddState(TEXT("Dash"))
			.OnStart(this, &CSheila::Dash_OnStart)
			.Tick(this, &CSheila::Dash_Tick)
			.OnExit(this, &CSheila::Dash_OnExit)
			.Transition(TEXT("Walk"), FSM_TRANSITION(L"Dash To Walk", this, &CSheila::DashToWalk))
		.Build();										    

	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_FSM"), TEXT("Com_FSM"),
		(CComponent**)&m_pFSMCom, &builder)))
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
