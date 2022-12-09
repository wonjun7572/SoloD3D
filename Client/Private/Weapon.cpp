#include "stdafx.h"
#include "..\Public\Weapon.h"
#include "GameInstance.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice,pContext)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	:CGameObject(rhs)
{
}

HRESULT CWeapon::Init_Prototype()
{
	return S_OK;
}

HRESULT CWeapon::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	return S_OK;
}

void CWeapon::Tick(_double TimeDelta)
{
}

void CWeapon::Late_Tick(_double TimeDelta)
{
}

HRESULT CWeapon::Render()
{
	return S_OK;
}

void CWeapon::Free()
{
	__super::Free();

	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
}
