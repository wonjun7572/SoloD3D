#include "stdafx.h"
#include "..\Public\Effect_Point.h"
#include "GameInstance.h"

Effect_Point::Effect_Point(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CEffect(pDevice, pContext)
{
}

Effect_Point::Effect_Point(const Effect_Point & rhs)
	: CEffect(rhs)
{
}

HRESULT Effect_Point::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT Effect_Point::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(CGameObject::Init(&GameObjectDesc)))
		return E_FAIL;

	if (nullptr != pArg)
	{
		memcpy(&m_ParticleDesc, pArg, sizeof(m_ParticleDesc));
	}
	else
	{
		XMStoreFloat4x4(&m_ParticleDesc.PivotMatrix, XMMatrixIdentity());
		m_ParticleDesc.pTargetTransform = nullptr;
	}

	return S_OK;
}

void Effect_Point::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void Effect_Point::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT Effect_Point::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	m_pVIBufferCom->Render();

	return S_OK;
}

void Effect_Point::Free()
{
	__super::Free();

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);

	if (m_bClone)
	{
		Safe_Release(m_ParticleDesc.pTargetTransform);
	}
}