#include "stdafx.h"
#include "..\Public\Effect_Trail.h"
#include "GameInstance.h"
#include "Player.h"

CEffect_Trail::CEffect_Trail(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{
}

CEffect_Trail::CEffect_Trail(const CEffect_Trail & rhs)
	:CEffect(rhs)
{
}

HRESULT CEffect_Trail::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Trail::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_strObjName = L"Effect_Trail";

	m_iCntX = 10;
	m_iCntZ = 1;

	m_pTrailVtx = new VTXNORTEX[m_iCntX * m_iCntZ];
	m_pVIBufferCom->Get_VtxInfo(m_pTrailVtx);
	m_fAlpha = 1.f;
	m_UVMoveFactor = _float2(0.f, 0.f);

	return S_OK;
}

void CEffect_Trail::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	if (m_iLocalCoordinates == 0)
	{
		m_vStart = _vec3(0.0f, 0.0f, 0.0f);
		m_vEnd = _vec3(0.0f, -2.5f, -2.2f);
	}
	else if (m_iLocalCoordinates == 1)
	{
		m_vStart = _vec3(0.0f, -3.0f, 0.0f);
		m_vEnd = _vec3(0.0f, -5.5f, -2.2f);
	}

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	CGameObject* pGameObject = pGameInstance->Find_GameObject(LEVEL_CHAP1, L"Layer_Player", L"Player");
	m_pTargetMatrix = static_cast<CPlayer*>(pGameObject)->Get_TransformCom()->Get_World4x4();
	RELEASE_INSTANCE(CGameInstance);

	m_vStart =	XMVector3TransformCoord(m_vStart, XMLoadFloat4x4(&m_pTargetMatrix));
	m_vEnd = XMVector3TransformCoord(m_vEnd, XMLoadFloat4x4(&m_pTargetMatrix));

	for (int i = 0; i < m_iCntX; ++i)
	{
		_vec3 Temp, Temp2;

		if (i == 0)
		{
			Temp = m_pTrailVtx[0].vPosition;
			Temp2 = m_pTrailVtx[m_iCntX].vPosition;


			m_pTrailVtx[0].vPosition = m_vStart;
			m_pTrailVtx[m_iCntX].vPosition = m_vEnd;
		}
		else
		{
			_vec3 vTemp, vTemp2;

			vTemp = m_pTrailVtx[i].vPosition;

			vTemp2 = m_pTrailVtx[m_iCntX + i].vPosition;


			m_pTrailVtx[i].vPosition = Temp;

			m_pTrailVtx[m_iCntX + i].vPosition = Temp2;

			Temp = vTemp;
			Temp2 = vTemp2;

		}
	}

	m_pVIBufferCom->Set_VtxInfo(m_pTrailVtx);
}

void CEffect_Trail::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_Trail::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CEffect_Trail::Imgui_RenderProperty()
{
}

HRESULT CEffect_Trail::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_TextureEffect"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_VIBuffer_Trail"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Texture_Arcane"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Trail::SetUp_ShaderResources()
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

	if (FAILED(m_pShaderCom->Set_ShaderResourceView("g_DepthTexture", pGameInstance->Get_DepthTargetSRV())))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_AlbedoTexture")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_UVMoveFactor", &m_UVMoveFactor, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CEffect_Trail * CEffect_Trail::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CEffect_Trail*		pInstance = new CEffect_Trail(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CEffect_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CEffect_Trail::Clone(void * pArg)
{
	CEffect_Trail*		pInstance = new CEffect_Trail(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CEffect_Trail");
		Safe_Release(pInstance);
	}
	return pInstance;
}


void CEffect_Trail::Free()
{
	__super::Free();

	if(m_bClone)
		Safe_Delete_Array(m_pTrailVtx);
}
