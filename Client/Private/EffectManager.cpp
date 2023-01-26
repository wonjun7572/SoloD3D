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

HRESULT CEffectManager::LinkObject(const wstring & pEffectTag, _float4 vTargetPos, _float4 vLook)
{
	CGameObject* pGameObject = Find_Effects(pEffectTag);

	if (nullptr == pGameObject)
		return E_FAIL;

	pGameObject->Get_TransformCom()->Set_State(CTransform::STATE_TRANSLATION, vTargetPos);
	return S_OK;
}

HRESULT CEffectManager::SetUp_Effects(const wstring & pEffectTag, _float fUVSpeed, _float fAlpha, _float2 UV)
{
	CGameObject* pGameObject = Find_Effects(pEffectTag);

	if (nullptr == pGameObject)
		return E_FAIL;

	static_cast<CEffect*>(pGameObject)->Set_Alpha(fAlpha);
	static_cast<CEffect*>(pGameObject)->Set_UVSpeed(fUVSpeed);
	static_cast<CEffect*>(pGameObject)->Set_UV(UV);

	return S_OK;
}

CTransform * CEffectManager::Get_Transform(const wstring & pEffectTag)
{
	CGameObject* pGameObject = Find_Effects(pEffectTag);

	if (nullptr == pGameObject)
		return nullptr;

	return pGameObject->Get_TransformCom();
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
