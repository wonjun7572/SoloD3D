#include "..\Public\Component_Manager.h"
#include "Component.h"
#include "GameUtils.h"

IMPLEMENT_SINGLETON(CComponent_Manager)

CComponent_Manager::CComponent_Manager()
{
}

HRESULT CComponent_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (nullptr != m_pPrototypes)
		return E_FAIL;

	m_pPrototypes = new PROTOTYPES[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CComponent_Manager::Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pPrototypes ||	iLevelIndex >= m_iNumLevels)
		return E_FAIL;

	if (nullptr != Find_Prototype(iLevelIndex, pPrototypeTag))
		return E_FAIL;

	m_pPrototypes[iLevelIndex].emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

CComponent * CComponent_Manager::Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void * pArg)
{
	CComponent* pPrototype = Find_Prototype(iLevelIndex, pPrototypeTag);
	if (pPrototype == nullptr)
		return nullptr;

	CComponent* pComponent = pPrototype->Clone(pArg);
	if (pComponent == nullptr)
		return nullptr;

	return pComponent;
}

void CComponent_Manager::Imgui_ComponentViewer(_uint iLevel, OUT const _tchar *& szSelectedProto)
{
	if (m_iNumLevels <= iLevel)
		return;

	const PROTOTYPES& targetLevel = m_pPrototypes[iLevel];

	if (ImGui::CollapsingHeader("ComponentViewer"))
	{
		for (auto& Pair : targetLevel)
		{
			char szComProtoName[MAX_PATH];

			const bool bSelected = (szComProtoName != nullptr) && (0 == lstrcmp(Pair.first.c_str(), szSelectedProto));
			if (bSelected)
				ImGui::SetItemDefaultFocus();

			char szViewName[512], szProtoName[256];
			CGameUtils::wc2c(Pair.first.c_str(), szProtoName);
			sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*Pair.second).name());

			if (ImGui::Selectable(szViewName, bSelected))
				szSelectedProto = Pair.first.c_str();
		}
	}
}

CComponent * CComponent_Manager::Find_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag)
{
	auto iter = m_pPrototypes[iLevelIndex].find(pPrototypeTag);
	
	if (iter == m_pPrototypes[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CComponent_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pPrototypes[i])
			Safe_Release(Pair.second);
		m_pPrototypes[i].clear();
	}

	Safe_Delete_Array(m_pPrototypes);
}
