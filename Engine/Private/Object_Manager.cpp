#include "..\Public\Object_Manager.h"

#include "Layer.h"
#include "GameObject.h"
#include "GameUtils.h"
#include "Transform.h"

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

void CObject_Manager::Imgui_ProtoViewer(_uint iLevel, OUT const _tchar *& szSelectedProto)
{
	if (m_iNumLevels <= iLevel)
		return;

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

		char szSelectProto[256] = {};
		CGameUtils::wc2c(szSelectedProto, szSelectProto);
		ImGui::Text("ProtoName: "); ImGui::SameLine();
		ImGui::Text(szSelectProto);

		ImGui::InputText("Layer Name", m_szLayerName, 256);
		string str(m_szLayerName);
		wstring strLayerName;
		strLayerName.assign(str.begin(), str.end());

		if (ImGui::Button("CLONE GAMEOBJECT"))
		{
			Clone_GameObject(iLevel, strLayerName, szSelectedProto);
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
					if (ImGui::Button("DELETE GAMEOBJECT"))
					{
						Pair.second->Delete_GameObject(pSelectedObject);
						Safe_Release(pSelectedObject);
					}
				}
				ImGui::TreePop();
			}
		}
		ImGui::TreePop();
	}

	if (bFound == false)
		pSelectedObject = nullptr;
}

void CObject_Manager::SaveData(_uint iLevel, wstring strDirectory)
{
	if (m_iNumLevels <= iLevel)
		return;

	HANDLE      hFile = CreateFile(strDirectory.c_str(),
		GENERIC_WRITE,        
		NULL,				  
		NULL,					
		CREATE_ALWAYS,        
		FILE_ATTRIBUTE_NORMAL,
		NULL);					

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	const LAYERS& targetLevel = m_pLayers[iLevel];
	DWORD dwByte = 0;
	
	_uint layerSize = static_cast<_uint>(targetLevel.size());
	WriteFile(hFile, &layerSize, sizeof(_uint), &dwByte, nullptr);
	for (auto& Pair : targetLevel) // for layer loop
	{
		_uint gameObjSize = static_cast<_uint>(Pair.second->GetGameObjects().size());
		WriteFile(hFile, &gameObjSize, sizeof(_uint), &dwByte, nullptr);
		
		_tchar szLayerName[256] = {};
		wcscpy_s(szLayerName, 256, Pair.first.c_str());
		WriteFile(hFile, &szLayerName, 256, &dwByte, nullptr);
		
		for (auto& obj : Pair.second->GetGameObjects())
		{
			char szName[256] = {};
			strcpy_s(szName, 256, typeid(*obj).name());
			WriteFile(hFile, &szName, 256 , &dwByte, nullptr);
		}
	}

	for (auto& Pair : targetLevel) // for layer loop
	{
		for (auto& obj : Pair.second->GetGameObjects())
		{
			auto iter = obj->GetComponents().find(TEXT("Com_Transform"));

			if (iter == obj->GetComponents().end())
				return;
			_matrix worldMatrix = dynamic_cast<CTransform*>(iter->second)->Get_WorldMatrix();
			WriteFile(hFile, &worldMatrix, sizeof(_matrix), &dwByte, nullptr);
		}
	}
			
	CloseHandle(hFile);
	MSG_BOX("Save_Complete!!!");
}

void CObject_Manager::LoadData(_uint iLevel, wstring strDirectory)
{
	if (m_iNumLevels <= iLevel)
		return;

	HANDLE      hFile = CreateFile(strDirectory.c_str(),    
		GENERIC_READ,         
		NULL,					
		NULL,					
		OPEN_EXISTING,        
		FILE_ATTRIBUTE_NORMAL,
		NULL);					

	if (INVALID_HANDLE_VALUE == hFile)
		return;

	const LAYERS& targetLevel = m_pLayers[iLevel];
	DWORD   dwByte = 0;

	_uint layerSize;
	ReadFile(hFile, &layerSize, sizeof(_uint), &dwByte, nullptr);
	for (_uint i = 0; i < layerSize; ++i)
	{
		_uint gameObjSize;
		ReadFile(hFile, &gameObjSize, sizeof(_uint), &dwByte, nullptr);

		_tchar szLayerName[256];
		ReadFile(hFile, &szLayerName, 256, &dwByte, nullptr);

		for (_uint j = 0; j < gameObjSize; ++j)
		{
			char szName[256];
			ReadFile(hFile, &szName, 256 , &dwByte, nullptr);

			for (auto& proto : m_Prototypes)
			{
				if (!strcmp(szName, typeid(*(proto.second)).name()))
				{
					Clone_GameObject(iLevel, szLayerName, proto.first);
				}
			}
		}
	}

	for (auto& Pair : targetLevel) // for layer loop
	{
		for (auto& obj : Pair.second->GetGameObjects())
		{
			auto iter = obj->GetComponents().find(TEXT("Com_Transform"));

			if (iter == obj->GetComponents().end())
				return;

			_matrix worldMatrix = XMMatrixIdentity();
			ReadFile(hFile, &worldMatrix, sizeof(_matrix), &dwByte, nullptr);
			_float4x4 world4x4;
			XMStoreFloat4x4(&world4x4, worldMatrix);
			dynamic_cast<CTransform*>(iter->second)->SetWorldMatrix(world4x4);
		}
	}

	CloseHandle(hFile);
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
