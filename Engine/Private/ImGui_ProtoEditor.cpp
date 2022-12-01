#include "..\Public\ImGui_ProtoEditor.h"

#include "GameObject.h"
#include "Level_Manager.h"
#include "GameInstance.h"

CImGui_ProtoEditor::CImGui_ProtoEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CImguiObject(pDevice, pContext)
{
}

HRESULT CImGui_ProtoEditor::Initialize(void * pArg)
{
	m_szTabname = "ProtoEditor";
	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
	return S_OK;
}

void CImGui_ProtoEditor::Imgui_RenderTab()
{
	//static const _tchar* test1 = L"";
	//m_pGameInstance->Imgui_ComponentViewer(CLevel_Manager::GetInstance()->GetCurLevelIdx(), test1);
}

CImGui_ProtoEditor * CImGui_ProtoEditor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	auto inst = new CImGui_ProtoEditor(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImGui_ProtoEditor::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
