#include "stdafx.h"
#include "..\Public\TestSphere.h"
#include "GameInstance.h"
#include "MathUtils.h"

CTestSphere::CTestSphere(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CTestSphere::CTestSphere(const CTestSphere & rhs)
	:CGameObject(rhs)
{
}

HRESULT CTestSphere::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestSphere::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Go_Left(1);

	return S_OK;
}

void CTestSphere::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);


	CGameInstance*			pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	m_pFSMCom->Tick(TimeDelta);

	/*if (pGameInstance->Get_DIKeyState(DIK_UPARROW))
		m_pTransformCom->Go_Straight(TimeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_DOWNARROW))
		m_pTransformCom->Go_Backward(TimeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_LEFTARROW))
		m_pTransformCom->Go_Left(TimeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_RIGHTARROW))
		m_pTransformCom->Go_Right(TimeDelta);
	
	if (pGameInstance->Get_DIKeyState(DIK_SPACE))
		m_pTransformCom->Go_Up(TimeDelta);

	if (pGameInstance->Get_DIKeyState(DIK_C))
		m_pTransformCom->Go_Down(TimeDelta);*/

	Safe_Release(pGameInstance);
}

void CTestSphere::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTestSphere::Render()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CTestSphere::Idle_OnStart()
{
	m_pTransformCom->Go_Left(1);
}

void CTestSphere::Idle_Tick(_double TimeDelta)
{
	m_pTransformCom->Go_Right(TimeDelta);
}

void CTestSphere::Idle_OnExit()
{
	m_pTransformCom->Go_Up(2);
}

void CTestSphere::Walk_Tick(_double TimeDelta)
{
	m_TimeDelta += TimeDelta * 0.5;

	if (m_TimeDelta >= 1)
		m_TimeDelta = 0;

	XMFLOAT4 f1 = XMFLOAT4(-20.f, 0.f, -20.f, 1.f);
	XMFLOAT4 f2 = XMFLOAT4(-10.f, 5.f, 10.f, 1.f);
	XMFLOAT4 f3 = XMFLOAT4(-10.f, 5.f, 10.f, 1.f);
	XMFLOAT4 f4 = XMFLOAT4(-200.f, 0.f, 100.f, 1.f);
	//m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&CMathUtils::SmoothStep(f1, f2, m_TimeDelta)));
	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&CMathUtils::CatmullRom(f1, f2, f3, f4, static_cast<float>(m_TimeDelta))));
	//static XMFLOAT4	Hermite(const XMFLOAT4& v1, const XMFLOAT4& t1, const XMFLOAT4& v2, const XMFLOAT4& t2, float t);
}

bool CTestSphere::Idle2Walk_KeyInput()
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_UPARROW))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	RELEASE_INSTANCE(CGameInstance);
	return false;
}

bool CTestSphere::Idle2Walk_Pushed()
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_UPARROW))
	{

		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	RELEASE_INSTANCE(CGameInstance);
	return false;
}

bool CTestSphere::Predic_Walk2Idle()
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	if (pGameInstance->Get_DIKeyState(DIK_DOWNARROW))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	RELEASE_INSTANCE(CGameInstance);
	return false;
}

HRESULT CTestSphere::SetUp_Components()
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
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Sphere"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Terrain"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* For. Com_FSM */
	CFSMComponentBuilder builder = CFSMComponentBuilder() // 빌더 생성
		.InitState(TEXT("Idle"))								  // 최초 시작 노드의 이름
		.AddState(TEXT("Idle"))								  // Idle 상태노드 정의 시작
			.OnStart(this, &CTestSphere::Idle_OnStart)	      // Idle 상태 시작할 때 CBackGround::Idle_OnStart함수 실행(상태당 하나의 함수 정의, 생략가능)
			.Tick(this,&CTestSphere::Idle_Tick)	      // Idle 상태 유지 될 때 프레임마다 CBackGround::Idle_Tick함수 실행(상태당 하나의 함수 정의, 생략가능)
			.OnExit(this, &CTestSphere::Idle_OnExit)          // Idle 상태에서 다른 상태로 이동할 때 실행하는 함수 정의(상태당 하나의 함수 정의, 생략가능)
			.Transition(TEXT("Walk"), FSM_TRANSITION(TEXT("Idle To Walk KeyInput"), this, &CTestSphere::Idle2Walk_KeyInput))
			// Idle에서 Walk로 전이하는 조건 정의, CBackGround::Predic_Idle2Walk함수 실행결과
			// true면 Walk로 전이한다. 이하 반복
			.Transition(TEXT("Walk"), FSM_TRANSITION(TEXT("Idle To Walk Pushed"), this, &CTestSphere::Idle2Walk_Pushed))
			// 다수 transition정의 가능
		.AddState(TEXT("Walk")) // Walk상태 노드 정의 시작
		.Tick(this,&CTestSphere::Walk_Tick)
		.Transition(TEXT("Idle"), FSM_TRANSITION(L"Walk To Idle", this, &CTestSphere::Predic_Walk2Idle))
		.Build();										      // 모든 상태를 만들면 Build()함수로 최종 Builder를 만든다.(종료함수)

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_FSM"), TEXT("Com_FSM"),
		(CComponent**)&m_pFSMCom, &builder)))			      // 만들어진 CFSMComponentBuilder의 포인터를 pArg로 넘겨준다.
		return E_FAIL;								  // 이렇게 넘겨진 클래스는 "이동" 되므로 다시 사용할 수 없다.
		
	return S_OK;
}

HRESULT CTestSphere::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_pTransformCom->Get_World4x4())))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof _float4)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_DiffuseTexture")))
		return E_FAIL;


	return S_OK;
}

CTestSphere * CTestSphere::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTestSphere*		pInstance = new CTestSphere(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestSphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTestSphere::Clone(void * pArg)
{
	CTestSphere*		pInstance = new CTestSphere(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestSphere");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestSphere::Free()
{
	__super::Free();
	Safe_Release(m_pFSMCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
