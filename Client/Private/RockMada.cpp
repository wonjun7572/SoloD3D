#include "stdafx.h"
#include "..\Public\RockMada.h"
#include "GameInstance.h"
#include "Particle.h"
#include "Player.h"
#include "PlayerCamera.h"

CRockMada::CRockMada(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CRockMada::CRockMada(const CRockMada & rhs)
	:CGameObject(rhs)
{
}

HRESULT CRockMada::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CRockMada::Init(void * pArg)
{
	ZeroMemory(&m_Desc, sizeof(DESC));

	if (pArg == nullptr)
		return E_FAIL;

	memcpy(&m_Desc, pArg, sizeof(DESC));

	CGameObject::GAMEOBJECTDESC GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);
	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.0f;

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	CParticle::DESC desc;
	ZeroMemory(&desc, sizeof(CParticle::DESC));

	desc.fShowTime = 0.f;
	desc.iProperty = CParticle::GODIRECTION;
	desc.fSizeUV_X = 1.f / 4.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 4;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_Explosion_Sub_08_White5";
	desc.ViBufferDesc.fMinSpeed = 0.1f;
	desc.ViBufferDesc.fMaxSpeed = 1.f;
	desc.ViBufferDesc.iNumSprite = 16;
	desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 1.024f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 1.024f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 1.2f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.3f;
	desc.ViBufferDesc.iNumInstances = 80;
	desc.ViBufferDesc.iNumSpreadInstances = 0;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	m_pParticle = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Particle"), &desc);
	
	ZeroMemory(&desc, sizeof(CParticle::DESC));

	desc.fShowTime =12.f;
	desc.iProperty = CParticle::COCHLEA;
	desc.fSizeUV_X = 1.f / 8.f;
	desc.fSizeUV_Y = 1.f / 4.f;
	desc.iSpriteCount_X = 8;
	desc.iSpriteCount_Y = 4;
	desc.pTextureTag = L"FX_SmokeDarkFlipBlend";
	desc.ViBufferDesc.fMinSpeed = 0.1f;
	desc.ViBufferDesc.fMaxSpeed = 1.f;
	desc.ViBufferDesc.iNumSprite = 32;
	desc.ViBufferDesc.vMaxSize = _float2(1.024f * 1.2f, 0.512f * 1.2f);
	desc.ViBufferDesc.vMinSize = _float2(1.024f * 0.7f, 0.512f * 0.7f);
	desc.ViBufferDesc.fMinTime = 0.1f;
	desc.ViBufferDesc.fMaxTime = 0.3f;
	desc.ViBufferDesc.fRange = 2.f;
	desc.ViBufferDesc.fRangeOffset = 1.f;
	desc.ViBufferDesc.fMinSpreadSizeOffset = 0.1f;
	desc.ViBufferDesc.fMaxSpreadSizeOffset = 0.5f;
	desc.ViBufferDesc.iNumInstances = 0;
	desc.ViBufferDesc.iNumSpreadInstances = 100;

	if (FAILED(pGameInstance->Clone_GameObject(pGameInstance->GetCurLevelIdx(), TEXT("Layer_Effect"), TEXT("Prototype_GameObject_Particle"), &desc, &m_pAfterParticle)))
		return E_FAIL;

	m_pTargetUI = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_DemonSKillFloor"));
	m_pTargetUI->Get_TransformCom()->Set_Scaled(_float3(5.f, 5.f, 1.f));
	pGameInstance->Play_Sound(L"RockMadaInit.mp3", 1.f,false, SOUND_ROCKMADA);
	pGameInstance->Play_Sound(L"RockMadaInit2.mp3", 1.f, false, SOUND_ROCKMADA);
	RELEASE_INSTANCE(CGameInstance);

	_float4 vPos = m_Desc.vDestination;

	vPos.y -= 30.f;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, vPos);

	if (g_LEVEL == LEVEL_CHAP1)
		m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Player", L"Player"));
	else if (g_LEVEL == LEVEL_CHAP2)
		m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_CHAP2, L"Layer_Player", L"Player"));
	else if (g_LEVEL == LEVEL_CHAP3)
		m_pPlayer = static_cast<CPlayer*>(pGameInstance->Find_GameObject(LEVEL_CHAP3, L"Layer_Player", L"Player"));

	m_fRandY = CMathUtils::GetRandomFloat(0.6f, 5.f);
	m_fRandTime = CMathUtils::GetRandomFloat(0.8f, 1.2f);

	return S_OK;
}

void CRockMada::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_pCam == nullptr)
	{
		CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
		m_pCam = static_cast<CPlayerCamera*>(pGameInstance->Find_GameObject(pGameInstance->GetCurLevelIdx(), L"Layer_Camera", L"PlayerCamera"));
		RELEASE_INSTANCE(CGameInstance);
	}
	else
		m_pCam->Shake(0.5f, 0.05f);

	_float4 vPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	m_pParticle->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION, vPos);
	
	static_cast<CParticle*>(m_pParticle)->Set_Direction(XMVectorSet(0.f,1.f,0.f,0.f));
	m_pParticle->Tick(TimeDelta);

	m_pTargetUI->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION, m_Desc.vDestination);
	m_pTargetUI->Tick(TimeDelta);

	_float4 vDestination = m_Desc.vDestination;
	vDestination.y -= m_fRandY;

	_float4 vAfterPos = m_Desc.vDestination;

	m_pAfterParticle->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION, vAfterPos);
	m_pTransformCom->Chase(vDestination, TimeDelta * m_fRandTime, 0.2f);

	if (_float4::Distance(vPos, vDestination) < 0.5f)
		m_fDissolveAmount += static_cast<_float>(TimeDelta) * 0.5f;

	if (m_fDissolveAmount >= 2.f)
	{
		if (!m_bDead)
		{
			CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance)
				pGameInstance->Play_Sound(L"RockMadaDie.mp3", 1.f, false, SOUND_ROCKMADA);
			RELEASE_INSTANCE(CGameInstance)
		}
		m_bDead = true;
	}
}

void CRockMada::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
	m_pParticle->Late_Tick(TimeDelta);

	if (m_fDissolveAmount <= -1.f)
		m_pTargetUI->Late_Tick(TimeDelta);

	m_pColliderCom->Update(m_pTransformCom->Get_WorldMatrix());

	if (nullptr != m_pRendererCom)
	{
		if (m_fDissolveAmount <= -1.f)
			m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_SHADOW, this);

		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
#ifdef _DEBUG 
		m_pRendererCom->Add_DebugRenderGroup(m_pColliderCom);
#endif
	}

	CollisionToAttack(TimeDelta);
}

HRESULT CRockMada::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		bool HasNormal = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_NORMALS, "g_NormalTexture")))
			HasNormal = false;
		else
			HasNormal = true;

		m_pShaderCom->Set_RawValue("g_HasNormal", &HasNormal, sizeof(bool));

		bool HasSpecular = false;
		if (FAILED(m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_SPECULAR, "g_SpecularTexture")))
			HasSpecular = false;
		else
			HasSpecular = true;

		m_pShaderCom->Set_RawValue("g_HasSpecular", &HasSpecular, sizeof(bool));

		m_pModelCom->Render(m_pShaderCom, i, 5);
	}

	return S_OK;
}

HRESULT CRockMada::RenderShadow()
{
	if (FAILED(__super::RenderShadow()))
		return E_FAIL;

	if (FAILED(SetUP_ShadowShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
		m_pModelCom->Render(m_pShaderCom, i, 4);

	return S_OK;
}

void CRockMada::Imgui_RenderProperty()
{
	ImGui::DragFloat("Dissolve", &m_fDissolveAmount, 0.01f, -10.f, 10.f);
	ImGui::DragFloat("Fringe", &m_fFringeAmount, 0.01f, -10.f, 10.f);
}

void CRockMada::CollisionToAttack(_double TimeDelta)
{
	CGameInstance*			pGameInstance = GET_INSTANCE(CGameInstance);

	CCollider*		pTargetCollider = nullptr;

	if (g_LEVEL == LEVEL_CHAP1)
		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP1, TEXT("Layer_Player"), TEXT("Com_AABB"));
	else if (g_LEVEL == LEVEL_CHAP2)
		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP2, TEXT("Layer_Player"), TEXT("Com_AABB"));
	else if (g_LEVEL == LEVEL_CHAP3)
		pTargetCollider = (CCollider*)pGameInstance->Get_ComponentPtr(LEVEL_CHAP3, TEXT("Layer_Player"), TEXT("Com_AABB"));

	if (nullptr == pTargetCollider)
		return;

	if (m_pColliderCom->Collision(pTargetCollider) == true)
	{
		// 플레이어가 앞을 보고 있느냐 뒤를 보고 있느냐를 판단하자
		_vector			vTargetLook = m_pPlayer->Get_TransformCom()->Get_State(CTransform::STATE_LOOK);
		_vector			vDot = XMVector3Dot(m_pTransformCom->Get_State(CTransform::STATE_LOOK), vTargetLook);
		_float			fDot = XMVectorGetX(vDot);

		// 0보다 작으면 내 앞에 있다.
		if (fDot < 0)
			m_pPlayer->FrontDamagedToMonster(10.f);
		else
			m_pPlayer->BackDamagedToMonster(10.f);
	}

	RELEASE_INSTANCE(CGameInstance);
}

HRESULT CRockMada::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Rock_Mada"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vCenter = _float3(0.f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(2.f, 2.f, 2.f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider"),
		(CComponent**)&m_pColliderCom, &ColliderDesc)))
		return E_FAIL;

	/* For.Com_DissolveTex */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Dissolve"), TEXT("Com_DissolveTex"),
		(CComponent**)&m_pDissolveTexCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CRockMada::SetUp_ShaderResources()
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
	if (FAILED(m_pShaderCom->Set_RawValue("g_vRimColor", &m_vRimColor, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("fDissolveAmount", &m_fDissolveAmount, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_RawValue("fFringeAmount", &m_fFringeAmount, sizeof(_float))))
		return E_FAIL;
	if (FAILED(m_pDissolveTexCom->Bind_ShaderResource(m_pShaderCom, "g_DissolveTexture")))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

HRESULT CRockMada::SetUP_ShadowShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance)

		if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
			return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance)

		return S_OK;
}

CRockMada * CRockMada::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRockMada*		pInstance = new CRockMada(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CRockMada");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CRockMada::Clone(void * pArg)
{
	CRockMada*		pInstance = new CRockMada(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CRockMada");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRockMada::Free()
{
	__super::Free();

	Safe_Release(m_pColliderCom);
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pParticle);
	Safe_Release(m_pTargetUI);
	Safe_Release(m_pDissolveTexCom);
	Safe_Release(m_pAfterParticle);
}
