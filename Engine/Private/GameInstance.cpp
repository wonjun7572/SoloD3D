#include "..\Public\GameInstance.h"

#include "Graphic_Device.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "ImGui_Manager.h"
#include "Timer_Manager.h"
#include "Font_Manager.h"
#include "Light_Manager.h"
#include "Frustum.h"
#include "Sound_Manager.h"

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
	m_pPipeLine(CPipeLine::GetInstance()),
	m_pTimerMgr(CTimer_Manager::GetInstance()),
	m_pFontMgr(CFont_Manager::GetInstance()),
	m_pLightMgr(CLight_Manager::GetInstance()),
	m_pFrustum(CFrustum::GetInstance()),
	m_pSoundMgr(CSound_Manager::GetInstance())
{
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pGraphicDev);
	Safe_AddRef(m_pInputDev);
	Safe_AddRef(m_pLevelMgr);
	Safe_AddRef(m_pObjectMgr);
	Safe_AddRef(m_pComponetMgr);
	Safe_AddRef(m_pImGuiMgr);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pTimerMgr);
	Safe_AddRef(m_pFontMgr);
	Safe_AddRef(m_pLightMgr);
	Safe_AddRef(m_pSoundMgr);
}

HRESULT CGameInstance::Init_Engine(_uint iNumLevels, const GRAPHIC_DESC & GraphicDesc, ID3D11Device ** ppDeviceOut, ID3D11DeviceContext ** ppContextOut)
{
	if (nullptr == m_pGraphicDev ||	nullptr == m_pInputDev ||nullptr == m_pObjectMgr || nullptr == m_pComponetMgr || m_pImGuiMgr == nullptr)
		return E_FAIL;

	m_hWnd = GraphicDesc.hWnd;

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

	if (FAILED(m_pSoundMgr->Reserve_Manager("../Bin/Resources/Sounds/", 10)))
		return E_FAIL;

	if (FAILED(m_pComponetMgr->Add_Prototype(m_iStaticLevelIndex, m_pPrototypeTransformTag, CTransform::Create(*ppDeviceOut, *ppContextOut))))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
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
	
	m_pSoundMgr->Tick(TimeDelta);

	m_pPipeLine->Tick();

	m_pFrustum->Transform_ToWorldSpace();

	m_pObjectMgr->Late_Tick(TimeDelta);
	m_pLevelMgr->Late_Tick(TimeDelta);
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

_bool CGameInstance::Mouse_Down(MOUSEKEYSTATE MouseButton, _bool* pData)
{
	if (m_pInputDev == nullptr)
		return false;

	return m_pInputDev->Mouse_Down(MouseButton, pData);
}

_bool CGameInstance::Mouse_Up(MOUSEKEYSTATE MouseButton, _bool* pData)
{
	if (m_pInputDev == nullptr)
		return false;

	return m_pInputDev->Mouse_Up(MouseButton, pData);
}

_bool CGameInstance::Key_Down(_ubyte byKeyID, _bool* pData)
{
	if (m_pInputDev == nullptr)
		return false;

	return m_pInputDev->Key_Down(byKeyID, pData);
}

_bool CGameInstance::Key_Up(_ubyte byKeyID, _bool* pData)
{
	if (m_pInputDev == nullptr)
		return false;

	return m_pInputDev->Key_Up(byKeyID, pData);
}

_bool CGameInstance::Key_Pressing(_ubyte byKeyID)
{
	if (m_pInputDev == nullptr)
		return false;

	return m_pInputDev->Key_Pressing(byKeyID);
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

_uint CGameInstance::GetCurLevelIdx()
{
	if (m_pLevelMgr == nullptr)
		return E_FAIL;

	return	m_pLevelMgr->GetCurLevelIdx();
}

CComponent * CGameInstance::Get_ComponentPtr(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObjectMgr)
		return nullptr;

	return m_pObjectMgr->Get_ComponentPtr(iLevelIndex, pLayerTag, pComponentTag, iIndex);
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

void CGameInstance::Imgui_ProtoViewer(_uint iLevel, const _tchar*& szSelectedProto)
{
	if (nullptr == m_pObjectMgr)
		return;
	m_pObjectMgr->Imgui_ProtoViewer(iLevel, szSelectedProto);
}

void CGameInstance::Imgui_ObjectViewer(_uint iLevel, CGameObject*& pSelectedObject)
{
	if (nullptr == m_pObjectMgr)
		return;
	m_pObjectMgr->Imgui_ObjectViewer(iLevel, pSelectedObject);
}

CGameObject * CGameInstance::Clone_GameObject(const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pObjectMgr)
		return nullptr;

	return m_pObjectMgr->Clone_GameObject(pPrototypeTag, pArg);
}

CGameObject * CGameInstance::Find_GameObject(_uint iLevelIndex, const wstring & pLayerTag, const wstring & strObjName)
{
	if (nullptr == m_pObjectMgr)
		return nullptr;
	
	return m_pObjectMgr->Get_GameObject(iLevelIndex, pLayerTag, strObjName);
}

const list<CGameObject*>* CGameInstance::Find_LayerList(_uint iLevelIndex, const wstring & pLayerTag)
{
	if (nullptr == m_pObjectMgr)
		return nullptr;

	return m_pObjectMgr->Get_LayerList(iLevelIndex, pLayerTag);
}

void CGameInstance::SaveData(_uint iLevel, const wstring& strDirectory)
{
	if (nullptr == m_pObjectMgr)
		return;

	m_pObjectMgr->SaveData(iLevel, strDirectory);
}

void CGameInstance::LoadData(_uint iLevel, const wstring& strDirectory)
{
	if (nullptr == m_pObjectMgr)
		return;
	
	m_pObjectMgr->LoadData(iLevel, strDirectory);
}

void CGameInstance::SaveMapObjectData(_uint iLevel, const wstring& pLayerTag, const wstring& strDirectory)
{
	if (nullptr == m_pObjectMgr)
		return;

	m_pObjectMgr->SaveMapObjectData(iLevel, pLayerTag, strDirectory);
}

void CGameInstance::LoadMapObjectData(_uint iLevel, const wstring& pLayerTag, const wstring& strDirectory)
{
	if (nullptr == m_pObjectMgr)
		return;

	m_pObjectMgr->LoadMapObjectData(iLevel, pLayerTag, strDirectory);
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

void CGameInstance::Imgui_ComponentViewer(_uint iLevel, const _tchar *& szSelectedProto)
{
	if (m_pComponetMgr == nullptr)
		return;

	m_pComponetMgr->Imgui_ComponentViewer(iLevel, szSelectedProto);
}

void CGameInstance::Imgui_ModelComponetViewer(_uint iLevel, OUT const _tchar *& szSelectedProto)
{
	if (m_pComponetMgr == nullptr)
		return;

	m_pComponetMgr->Imgui_ModelComponetViewer(iLevel, szSelectedProto);
}

void CGameInstance::Remove_ProtoComponent(_uint iLevel, const wstring & pComponentName)
{
	if (m_pComponetMgr == nullptr)
		return;

	m_pComponetMgr->Remove_ProtoComponent(iLevel, pComponentName);
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

void CGameInstance::Add_ImguiObject(CImguiObject* pImguiObject)
{
	if (m_pImGuiMgr == nullptr) return;
	m_pImGuiMgr->Add_ImguiObject(pImguiObject);
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

_float4x4 CGameInstance::Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return  _float4x4();

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

_double CGameInstance::Get_TimeDelta(const wstring & pTimerTag)
{
	if (m_pTimerMgr == nullptr)
		return 0.0;

	return m_pTimerMgr->Get_TimeDelta(pTimerTag);
}

HRESULT CGameInstance::Ready_Timer(const wstring & pTimerTag)
{
	if (m_pTimerMgr == nullptr)
		return E_FAIL;

	return m_pTimerMgr->Ready_Timer(pTimerTag);
}

void CGameInstance::Update_Timer(const wstring & pTimerTag)
{
	if (m_pTimerMgr == nullptr)
		return;

	return m_pTimerMgr->Update_Timer(pTimerTag);
}

CTimer* CGameInstance::Find_Timer(const wstring& pTimerTag)
{
	if (m_pTimerMgr == nullptr)
		return nullptr;

	return m_pTimerMgr->Find_Timer(pTimerTag);
}

HRESULT CGameInstance::Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & pFontTag, const wstring & pFontFilePath)
{
	if (nullptr == m_pFontMgr)
		return E_FAIL;

	return m_pFontMgr->Add_Fonts(pDevice, pContext, pFontTag, pFontFilePath);
}

HRESULT CGameInstance::Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor)
{
	if (nullptr == m_pFontMgr)
		return E_FAIL;

	return m_pFontMgr->Render_Font(pFontTag, pText, vPos, fRadian, vScale, vColor);
}

const LIGHTDESC * CGameInstance::Get_LightDesc(_uint iIndex)
{
	if (m_pLightMgr == nullptr)
		return nullptr;

	return m_pLightMgr->Get_LightDesc(iIndex);
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	if (m_pLightMgr == nullptr)
		return E_FAIL;

	return m_pLightMgr->Add_Light(pDevice, pContext, LightDesc);
}

_bool CGameInstance::isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isInFrustum_WorldSpace(vWorldPos, fRange);
}

_bool CGameInstance::isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isInFrustum_LocalSpace(vLocalPos, fRange);
}

void CGameInstance::Play_Sound(const _tchar * pSoundKey, _float fVolume, _bool bIsBGM, _int iManualChannelIndex)
{
	if (nullptr == m_pSoundMgr)
		return;

	m_pSoundMgr->Play_Sound(pSoundKey, fVolume, bIsBGM, iManualChannelIndex);
}

void CGameInstance::Stop_Sound(_uint iManualChannelIndex)
{
	if (nullptr == m_pSoundMgr)
		return;

	m_pSoundMgr->Stop_Sound(iManualChannelIndex);
}

void CGameInstance::Set_Volume(_uint iManualChannelIndex, _float fVolume)
{
	if (nullptr == m_pSoundMgr)
		return;

	m_pSoundMgr->Set_Volume(iManualChannelIndex, fVolume);
}

void CGameInstance::Set_MasterVolume(_float fVolume)
{
	if (nullptr == m_pSoundMgr)
		return;

	m_pSoundMgr->Set_MasterVolume(fVolume);
}

void CGameInstance::Set_SoundDesc(const _tchar * pSoundKey, CSound::SOUND_DESC & SoundDesc)
{
	if (nullptr == m_pSoundMgr)
		return;

	m_pSoundMgr->Set_SoundDesc(pSoundKey, SoundDesc);
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();

	CLevel_Manager::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CInput_Device::GetInstance()->DestroyInstance();

	CFont_Manager::GetInstance()->DestroyInstance();

	CLight_Manager::GetInstance()->DestroyInstance();

	CFrustum::GetInstance()->DestroyInstance();

	CSound_Manager::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();

	CTimer_Manager::GetInstance()->DestroyInstance();

	CImGui_Manager::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pFrustum);
	Safe_Release(m_pImGuiMgr);
	Safe_Release(m_pComponetMgr);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pTimerMgr);
	Safe_Release(m_pFontMgr);
	Safe_Release(m_pObjectMgr);
	Safe_Release(m_pLevelMgr);
	Safe_Release(m_pInputDev);
	Safe_Release(m_pLightMgr);
	Safe_Release(m_pSoundMgr);
	Safe_Release(m_pGraphicDev);
}
