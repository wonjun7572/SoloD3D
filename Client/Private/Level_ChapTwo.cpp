#include "stdafx.h"
#include "..\Public\Level_ChapTwo.h"

CLevel_ChapTwo::CLevel_ChapTwo(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT Client::CLevel_ChapTwo::Init()
{
	if (FAILED(CLevel::Init()))
		return E_FAIL;

	return S_OK;
}

void Client::CLevel_ChapTwo::Tick(_double TimeDelta)
{
	CLevel::Tick(TimeDelta);
}

void Client::CLevel_ChapTwo::Late_Tick(_double TimeDelta)
{
	CLevel::Late_Tick(TimeDelta);
}

HRESULT Client::CLevel_ChapTwo::Render()
{
	if (FAILED(CLevel::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, TEXT("Level : CHAPTER 2"));

	return S_OK;
}

CLevel_ChapTwo * CLevel_ChapTwo::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CLevel_ChapTwo*		pInstance = new CLevel_ChapTwo(pDevice, pContext);

	if (FAILED(pInstance->Init()))
	{
		MSG_BOX("Failed to Created : CLevel_ChapTwo");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void Client::CLevel_ChapTwo::Free()
{
	CLevel::Free();
}
