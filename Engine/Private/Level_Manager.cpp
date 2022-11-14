#include "..\Public\Level_Manager.h"
#include "Level.h"

#include "GameInstance.h"

IMPLEMENT_SINGLETON(CLevel_Manager)

CLevel_Manager::CLevel_Manager()
{
}

HRESULT CLevel_Manager::OpenLevel(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (pNewLevel == nullptr)
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (nullptr != m_pCurrentLevel)
		pGameInstance->Clear_Level(m_iLevelIndex);

	Safe_Release(m_pCurrentLevel);

	m_pCurrentLevel = pNewLevel;
	m_iLevelIndex = iLevelIndex;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CLevel_Manager::Tick(_double TimeDelta)
{
	if (m_pCurrentLevel == nullptr)
	{
		assert(m_pCurrentLevel != nullptr && "CLevel_Manager : Tick");
		return;
	}

	m_pCurrentLevel->Tick(TimeDelta);
}

void CLevel_Manager::Late_Tick(_double TimeDelta)
{
	if (m_pCurrentLevel == nullptr)
	{
		assert(m_pCurrentLevel != nullptr && "CLevel_Manager : Late_Tick");
		return;
	}

	m_pCurrentLevel->Late_Tick(TimeDelta);
}

HRESULT CLevel_Manager::Render()
{
	if (nullptr == m_pCurrentLevel)
		return E_FAIL;

	return m_pCurrentLevel->Render();
}

void CLevel_Manager::Free()
{
	Safe_Release(m_pCurrentLevel);
}
