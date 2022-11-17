#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "ImGui_Manager.h"

IMPLEMENT_SINGLETON(CGameInstance);

_uint CGameInstance::m_iStaticLevelIndex = 0;
wstring CGameInstance::m_pPrototypeTransformTag = TEXT("Prototype_Component_Transform");

CGameInstance::CGameInstance()
	:m_pGraphicDev(CGraphic_Device::GetInstance()),
	m_pInputDev(CInput_Device::GetInstance()),
	m_pLevelMgr(CLevel_Manager::GetInstance()),
	m_pObjectMgr(CObject_Manager::GetInstance()),
	m_pComponetMgr(CComponent_Manager::GetInstance()),
	m_pImGuiMgr(CImGui_Manager::GetInstance()),
	m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pGraphicDev);
	Safe_AddRef(m_pInputDev);
	Safe_AddRef(m_pLevelMgr);
	Safe_AddRef(m_pObjectMgr);
	Safe_AddRef(m_pComponetMgr);
	Safe_AddRef(m_pImGuiMgr);
	Safe_AddRef(m_pPipeLine);
}

HRESULT CGameInstance::Init_Engine(_uint iNumLevels, const GRAPHIC_DESC & GraphicDesc, ID3D11Device ** ppDeviceOut, ID3D11DeviceContext ** ppContextOut)
{
	if (nullptr == m_pGraphicDev ||	nullptr == m_pInputDev ||nullptr == m_pObjectMgr || nullptr == m_pComponetMgr || m_pImGuiMgr == nullptr)
		return E_FAIL;

	if (FAILED(m_pGraphicDev->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iViewportSizeX, GraphicDesc.iViewportSizeY, ppDeviceOut, ppContextOut)))
		return E_FAIL;

	CoInitializeEx(nullptr, COINIT_MULTITHREADED);

	m_pImGuiMgr->Ready_Imgui(GraphicDesc.hWnd, *ppDeviceOut, *ppContextOut);
		
	if (FAILED(m_pInputDev->Ready_Input_Device(GraphicDesc.hInstance, GraphicDesc.hWnd)))
		return E_FAIL;

	m_iStaticLevelIndex = iNumLevels;

	if (FAILED(m_pObjectMgr->Reserve_Manager(iNumLevels + 1)))
		return E_FAIL;

	if (FAILED(m_pComponetMgr->Reserve_Manager(iNumLevels + 1)))
		return E_FAIL;

	if (FAILED(m_pComponetMgr->Add_Prototype(m_iStaticLevelIndex, m_pPrototypeTransformTag, CTransform::Create(*ppDeviceOut, *ppContextOut))))
		return E_FAIL;

	return S_OK;
}

void CGameInstance::Tick_Engine(_double TimeDelta)
{
	if (m_pInputDev == nullptr || m_pLevelMgr == nullptr || m_pObjectMgr == nullptr)
	{
		assert((m_pInputDev != nullptr && m_pLevelMgr != nullptr && m_pObjectMgr != nullptr) && "CGameInstance : Tick_Engine");
		return;
	}

	m_pInputDev->Invalidate_Input_Device();

	m_pImGuiMgr->Tick_Imgui();

	m_pObjectMgr->Tick(TimeDelta);
	m_pLevelMgr->Tick(TimeDelta);
	
	m_pPipeLine->Tick();

	m_pObjectMgr->Late_Tick(TimeDelta);
	m_pLevelMgr->Late_Tick(TimeDelta);

	m_pInputDev->Reset_EveryKey();
}

void CGameInstance::Clear_Level(_uint iLevelIndex)
{
	if (nullptr == m_pObjectMgr)
	{
		assert((m_pObjectMgr != nullptr) && "CGameInstance : Clear_Level");
		return;
	}

	m_pObjectMgr->Clear(iLevelIndex);
}

HRESULT CGameInstance::Clear_Graphic_Device(const _float4 * pColor)
{
	if (m_pGraphicDev == nullptr)
		return E_FAIL;

	HRESULT hr = 0;
	
	hr = m_pGraphicDev->Clear_BackBuffer_View(*pColor);
	hr = m_pGraphicDev->Clear_DepthStencil_View();
	
	return hr;
}

HRESULT CGameInstance::Present()
{
	if (m_pGraphicDev == nullptr)
		return E_FAIL;

	return m_pGraphicDev->Present();
}

HRESULT CGameInstance::Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate)
{
	if (m_pGraphicDev == nullptr)
		return E_FAIL;

	return m_pGraphicDev->Update_SwapChain(hWnd, iWinCX, iWinCY, bIsFullScreen, bNeedUpdate);
}

_byte CGameInstance::Get_DIKeyState(_ubyte byKeyID)
{
	if (m_pInputDev == nullptr)
		return 0;

	return m_pInputDev->Get_DIKeyState(byKeyID);
}

_byte CGameInstance::Get_DIMouseState(MOUSEKEYSTATE byMouseID)
{
	if (m_pInputDev == nullptr)
		return 0;

	return m_pInputDev->Get_DIMouseState(byMouseID);
}

_long CGameInstance::Get_DIMouseMove(MOUSEMOVESTATE eMoveState)
{
	if (m_pInputDev == nullptr)
		return 0;

	return m_pInputDev->Get_DIMouseMove(eMoveState);
}

_bool CGameInstance::Mouse_Down(MOUSEKEYSTATE MouseButton)
{
	if (m_pInputDev == nullptr)
		return false;

	return m_pInputDev->Mouse_Down(MouseButton);
}

_bool CGameInstance::Mouse_Up(MOUSEKEYSTATE MouseButton)
{
	if (m_pInputDev == nullptr)
		return false;

	return m_pInputDev->Mouse_Up(MouseButton);
}

_bool CGameInstance::Mouse_DoubleClick(MOUSEKEYSTATE MouseButton)
{
	if (m_pInputDev == nullptr)
		return false;

	return m_pInputDev->Mouse_DoubleClick(MouseButton);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID)
{
	if (m_pInputDev == nullptr)
		return false;

	return m_pInputDev->Key_Down(byKeyID);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID)
{
	if (m_pInputDev == nullptr)
		return false;

	return m_pInputDev->Key_Up(byKeyID);
}

void CGameInstance::Reset_EveryKey()
{
	if (m_pInputDev == nullptr)
		return;

	m_pInputDev->Reset_EveryKey();
}

HRESULT CGameInstance::OpenLevel(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (m_pLevelMgr == nullptr)
		return E_FAIL;

	return 	m_pLevelMgr->OpenLevel(iLevelIndex, pNewLevel);
}

HRESULT CGameInstance::RenderLevel()
{
	if (m_pLevelMgr == nullptr)
		return E_FAIL;

	return	m_pLevelMgr->Render();
}

HRESULT CGameInstance::Add_Prototype(const wstring& pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObjectMgr)
		return E_FAIL;

	return m_pObjectMgr->Add_Prototype(pPrototypeTag, pPrototype);
}

HRESULT CGameInstance::Clone_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObjectMgr)
		return E_FAIL;

	return m_pObjectMgr->Clone_GameObject(iLevelIndex, pLayerTag, pPrototypeTag, pArg);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, CComponent * pPrototype)
{
	if (m_pComponetMgr == nullptr)
		return E_FAIL;

	return m_pComponetMgr->Add_Prototype(iLevelIndex, pPrototypeTag,pPrototype);
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void * pArg)
{
	if (m_pComponetMgr == nullptr)
		return nullptr;

	return m_pComponetMgr->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

void CGameInstance::Render_ImGui()
{
	if (nullptr == m_pImGuiMgr)
		return;

	m_pImGuiMgr->Render_Imgui();
}

void CGameInstance::Render_Update_ImGui()
{
	if (nullptr == m_pImGuiMgr)
		return;

	m_pImGuiMgr->Render_Update_ImGui();
}

void CGameInstance::Add_ImguiTabObject(CImguiObject* ImguiObject)
{
	if (m_pImGuiMgr == nullptr) return;
	m_pImGuiMgr->Add_ImguiTabObject(ImguiObject);
}

void CGameInstance::Add_ImguiWindowObject(CImguiObject* ImguiObject)
{
	if (m_pImGuiMgr == nullptr) return;
	m_pImGuiMgr->Add_ImguiWindowObject(ImguiObject);
}

void CGameInstance::Clear_ImguiObjects()
{
	if (m_pImGuiMgr == nullptr) return;
	m_pImGuiMgr->Clear_ImguiObjects();
}

_matrix CGameInstance::Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix(eState);
}

_float4x4 CGameInstance::Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_TransformFloat4x4(eState);
}

_matrix CGameInstance::Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_TransformMatrix_Inverse(eState);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_float4 CGameInstance::Get_CamPosition()
{
	if (nullptr == m_pPipeLine)
		return _float4();

	return m_pPipeLine->Get_CamPosition();
}

void CGameInstance::Release_Engine()
{
	CImGui_Manager::GetInstance()->DestroyInstance();

	CGameInstance::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CInput_Device::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pComponetMgr);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pLevelMgr);
	Safe_Release(m_pInputDev);
	Safe_Release(m_pImGuiMgr);
	Safe_Release(m_pGraphicDev);
}
