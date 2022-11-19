#include "..\Public\Timer_Manager.h"

IMPLEMENT_SINGLETON(CTimer_Manager)

CTimer_Manager::CTimer_Manager()
{
}

_double CTimer_Manager::Get_TimeDelta(const wstring & pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);
	
	if (pTimer == nullptr)
		return 0.0;

	return pTimer->Get_TimeDelta();
}

HRESULT CTimer_Manager::Ready_Timer(const wstring & pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (pTimer != nullptr)
		return E_FAIL;

	pTimer = CTimer::Create();
	
	if (pTimer == nullptr)
		return E_FAIL;

	m_mapTimers.insert({ pTimerTag, pTimer });

	return S_OK;
}

void CTimer_Manager::Update_Timer(const wstring & pTimerTag)
{
	CTimer* pTimer = Find_Timer(pTimerTag);

	if (pTimer == nullptr)
		return;

	pTimer->Update_Timer();
}

CTimer * CTimer_Manager::Find_Timer(const wstring & pTimerTag)
{
	auto iter = m_mapTimers.find(pTimerTag);

	if (iter == m_mapTimers.end())
		return nullptr;

	return iter->second;
}

void CTimer_Manager::Free(void)
{
	for (auto& Pair : m_mapTimers)
		Safe_Release(Pair.second);

	m_mapTimers.clear();
}
