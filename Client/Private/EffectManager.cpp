#include "stdafx.h"
#include "..\Public\EffectManager.h"
#include "Effect.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CEffectManager)

CEffectManager::CEffectManager()
{
}

HRESULT CEffectManager::Add_Effects(const wstring& pPrototypeEffectTag, const wstring & pEffectTag, void* pArg)
{
	if (nullptr != Find_Effects(pEffectTag))
		return E_FAIL;

	CGameObject* pGameObject = nullptr;

	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	pGameObject = pGameInstance->Clone_GameObject(pPrototypeEffectTag.c_str(), pArg);
	RELEASE_INSTANCE(CGameInstance);

	if (pGameObject == nullptr)
		return E_FAIL;

	// 😊이거 없애고 아래에서 safe release 하니까 되는 거 같아~
	//Safe_AddRef(pGameObject);
	m_Effects.insert({ pEffectTag, pGameObject });

	return S_OK;
}

HRESULT CEffectManager::Render_Effects(const wstring & pEffectTag, _double TimeDelta)
{
	CGameObject* pGameObject = Find_Effects(pEffectTag);

	if (nullptr == pGameObject)
		return E_FAIL;
	
	pGameObject->Tick(TimeDelta);
	pGameObject->Late_Tick(TimeDelta);
	
	return S_OK;
}

CGameObject * CEffectManager::Find_Effects(const wstring & pEffectTag)
{
	auto iter = m_Effects.find(pEffectTag);

	if (iter == m_Effects.end())
		return nullptr;

	return iter->second;
}

void CEffectManager::Free()
{
	for (auto& Pair : m_Effects)
		Safe_Release(Pair.second);

	m_Effects.clear();
}
