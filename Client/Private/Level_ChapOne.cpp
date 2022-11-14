#include "stdafx.h"
#include "..\Public\Level_ChapOne.h"

#include "GameInstance.h"

CLevel_ChapOne::CLevel_ChapOne(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT Client::CLevel_ChapOne::Init()
{
	if (FAILED(CLevel::Init()))
		return E_FAIL;

	if (FAILED(Ready_Layer_BackGround(TEXT("Layer_BackGround"))))
		return E_FAIL;

	return S_OK;
}

void Client::CLevel_ChapOne::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void Client::CLevel_ChapOne::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT Client::CLevel_ChapOne::Render()
{
	if (FAILED(CLevel::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : CHAPTER 1"));

	return S_OK;
}

HRESULT CLevel_ChapOne::Ready_Layer_BackGround(const wstring& pLayerTag)
{
	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Clone_GameObject(LEVEL_CHAP1, pLayerTag, TEXT("Prototype_GameObject_BackGround"))))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

CLevel_ChapOne * CLevel_ChapOne::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_ChapOne*		pInstance = new CLevel_ChapOne(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Created : CLevel_ChapOne");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Client::CLevel_ChapOne::Free()
{
	CLevel::Free();
}
