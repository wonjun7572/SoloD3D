#include "..\Public\Imgui_MapEditor.h"
#include "GameObject.h"
#include "Level_Manager.h"
#include "GameInstance.h"


CImgui_MapEditor::CImgui_MapEditor(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CImguiObject(pDevice, pContext)
{
}

HRESULT CImgui_MapEditor::Initialize(void * pArg)
{
	m_szTabname = "MapEditor";

	m_pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(m_pGameInstance);
	return S_OK;
}

void CImgui_MapEditor::Imgui_RenderTab()
{

}

CImgui_MapEditor * CImgui_MapEditor::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, void * pArg)
{
	auto inst = new CImgui_MapEditor(pDevice, pContext);
	if (FAILED(inst->Initialize(pArg)))
	{
		return nullptr;
	}

	return inst;
}

void CImgui_MapEditor::Free()
{
	__super::Free();
	Safe_Release(m_pGameInstance);
}
