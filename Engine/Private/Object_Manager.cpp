#include "..\Public\Object_Manager.h"

#include "Layer.h"
#include "GameObject.h"
#include "GameUtils.h"

IMPLEMENT_SINGLETON(CObject_Manager);

CObject_Manager::CObject_Manager()
{
}

HRESULT CObject_Manager::Reserve_Manager(_uint iNumLevels)
{
	if (m_pLayers != nullptr)
		return E_FAIL;

	m_pLayers = new LAYERS[iNumLevels];

	m_iNumLevels = iNumLevels;

	return S_OK;
}

HRESULT CObject_Manager::Clear(_uint iLevelIndex)
{
	if (m_iNumLevels <= iLevelIndex)
		return E_FAIL;

	for (auto& Pair : m_pLayers[iLevelIndex])
		Safe_Release(Pair.second);

	m_pLayers[iLevelIndex].clear();

	return S_OK;
}

HRESULT CObject_Manager::Add_Prototype(const wstring& pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr != Find_Prototype(pPrototypeTag))
	{
		Safe_Release(pPrototype);
		return E_FAIL;
	}

	m_Prototypes.emplace(pPrototypeTag, pPrototype);

	return S_OK;
}

HRESULT CObject_Manager::Clone_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, void * pArg)
{
	CGameObject* pPrototype = Find_Prototype(pPrototypeTag);
	
	if (pPrototype == nullptr)
		return E_FAIL;

	CGameObject* pGameObject = pPrototype->Clone(pArg);

	CLayer* pLayer = Find_Layer(iLevelIndex, pLayerTag);

	if (pLayer == nullptr)
	{
		pLayer = CLayer::Create();

		if (pLayer == nullptr)
			return E_FAIL;

		if (FAILED(pLayer->Add_GameObject(pGameObject)))
			return E_FAIL;

		m_pLayers[iLevelIndex].emplace(pLayerTag, pLayer);
	}
	else
		pLayer->Add_GameObject(pGameObject);

	return S_OK;
}

void CObject_Manager::Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (Pair.second != nullptr)
				Pair.second->Tick(TimeDelta);
		}
	}
}

void CObject_Manager::Late_Tick(_double TimeDelta)
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
		{
			if (Pair.second != nullptr)
				Pair.second->Late_Tick(TimeDelta);
		}
	}
}

void CObject_Manager::Imgui_ProtoViewer(OUT const _tchar *& szSelectedProto)
{
	if (ImGui::CollapsingHeader("ProtoViewer"))
	{
		if (ImGui::BeginListBox("##"))
		{
			for (auto& ProtoPair : m_Prototypes)
			{
				const bool bSelected = (szSelectedProto != nullptr) && (0 == lstrcmp(ProtoPair.first.c_str(), szSelectedProto));
				if (bSelected)
					ImGui::SetItemDefaultFocus();

				char szViewName[512], szProtoName[256];
				CGameUtils::wc2c(ProtoPair.first.c_str(), szProtoName);
				sprintf_s(szViewName, "%s [%s]", szProtoName, typeid(*ProtoPair.second).name());
				if (ImGui::Selectable(szViewName, bSelected))
					szSelectedProto = ProtoPair.first.c_str();
			}

			ImGui::EndListBox();
		}
	}
}

void CObject_Manager::Imgui_ObjectViewer(_uint iLevel, OUT CGameObject *& pSelectedObject)
{
	bool bFound = false;
	if (m_iNumLevels <= iLevel)
		return;

	const LAYERS& targetLevel = m_pLayers[iLevel];

	if (ImGui::TreeNode("ObjectViewer"))
	{
		for (auto& Pair : targetLevel) // for layer loop
		{
			char szLayerTag[128];
			CGameUtils::wc2c(Pair.first.c_str(), szLayerTag);
			if (ImGui::TreeNode(szLayerTag))  // for object loop listbox
			{
				if (ImGui::BeginListBox("##"))
				{
					for (auto& obj : Pair.second->GetGameObjects())
					{
						const bool bSelected = pSelectedObject == obj;

						if (bSelected)
						{
							ImGui::SetItemDefaultFocus();
							bFound = true;
						}

						char szAddressName[256];
						sprintf_s(szAddressName, "%s[%p]", typeid(*obj).name(), obj);

						if (ImGui::Selectable(szAddressName, bSelected))
						{
							pSelectedObject = obj;
							bFound = true;
						}
					}
					ImGui::EndListBox();
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if (bFound == false)
		pSelectedObject = nullptr;
}

CGameObject * CObject_Manager::Find_Prototype(const wstring& pPrototypeTag)
{
	auto iter = m_Prototypes.find(pPrototypeTag);

	if (iter == m_Prototypes.end())
		return nullptr;

	return iter->second;
}

CLayer * CObject_Manager::Find_Layer(_uint iLevelIndex, const wstring& pLayerTag)
{
	auto iter = m_pLayers[iLevelIndex].find(pLayerTag);
	
	if (iter == m_pLayers[iLevelIndex].end())
		return nullptr;

	return iter->second;
}

void CObject_Manager::Free()
{
	for (_uint i = 0; i < m_iNumLevels; ++i)
	{
		for (auto& Pair : m_pLayers[i])
			Safe_Release(Pair.second);

		m_pLayers[i].clear();
	}

	Safe_Delete_Array(m_pLayers);

	for (auto& Pair : m_Prototypes)
		Safe_Release(Pair.second);

	m_Prototypes.clear();
}
