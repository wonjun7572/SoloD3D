#include "stdafx.h"
#include "..\Public\Level_Loading.h"

#include "GameInstance.h"
#include "Loader.h"

#include "Level_Logo.h"
#include "Level_ChapOne.h"
#include "Level_ChapTwo.h"
#include "Level_Tool.h"

CLevel_Loading::CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CLevel(pDevice, pContext)
{
}

HRESULT CLevel_Loading::Init(LEVEL eNextLevelID)
{
	if (FAILED(__super::Init()))
		return E_FAIL;

	m_eNextLevelID = eNextLevelID;

	/* eNextLevelID를 위한 자원을 쓰레드에게 준비해라. */
	m_pLoader = CLoader::Create(m_pDevice, m_pContext, eNextLevelID);
	if (nullptr == m_pLoader)
		return E_FAIL;

	/* 로딩씬 구성을 위한 객체를 생성한다. */
	/* 로딩바. 배경. 등등등. */

	return S_OK;
}

void CLevel_Loading::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CLevel_Loading::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (true == m_pLoader->isFinished())
	{
		CLevel*		pLevel = nullptr;

		switch (m_eNextLevelID)
		{
		case LEVEL_LOGO:
			pLevel = CLevel_Logo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_CHAP1:
			pLevel = CLevel_ChapOne::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_CHAP2:
			pLevel = CLevel_ChapTwo::Create(m_pDevice, m_pContext);
			break;
		case LEVEL_TOOL:
			pLevel = CLevel_Tool::Create(m_pDevice, m_pContext);
			break;
		}

		if (nullptr == pLevel)
		{
			assert(pLevel != nullptr && "CLevel_Loading::Late_Tick");
			Safe_Release(pGameInstance);
			return;
		}

		if (FAILED(pGameInstance->OpenLevel(m_eNextLevelID, pLevel)))
		{
			Safe_Release(pGameInstance);
			return;
		}
	}

	Safe_Release(pGameInstance);
}

HRESULT CLevel_Loading::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	SetWindowText(g_hWnd, m_pLoader->Get_LoadingText().c_str());

	return S_OK;
}

CLevel_Loading * CLevel_Loading::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID)
{
	CLevel_Loading*		pInstance = new CLevel_Loading(pDevice, pContext);

	if (FAILED(pInstance->Init(eNextLevelID)))
	{
		MSG_BOX("Failed to Created : CLevel_Loading");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CLevel_Loading::Free()
{
	__super::Free();
	Safe_Release(m_pLoader);
}
