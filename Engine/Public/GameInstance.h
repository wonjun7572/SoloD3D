#pragma once

#include "Base.h"
#include "Component_Manager.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public:
	static _uint Get_StaticLevelIndex() { return m_iStaticLevelIndex; }
	static wstring m_pPrototypeTransformTag;
	HWND GetHWND() { return m_hWnd; }

public: // GameInstance
	HRESULT		Init_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut);
	void		Tick_Engine(_double TimeDelta);
	void		Clear_Level(_uint iLevelIndex);

public: // Graphic_Device
	HRESULT				 Clear_Graphic_Device(const _float4* pColor);
	HRESULT				 Present();
	HRESULT				 Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate);

public:		/* For InputDevice */
	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(MOUSEKEYSTATE byMouseID);
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMoveState);
	_bool		Mouse_Down(MOUSEKEYSTATE MouseButton, _bool* pData = nullptr);
	_bool		Mouse_Up(MOUSEKEYSTATE MouseButton, _bool* pData = nullptr);
	_bool		Key_Down(_ubyte byKeyID, _bool* pData = nullptr);
	_bool		Key_Up(_ubyte byKeyID, _bool* pData = nullptr);
	
public: // Level_Manager
	HRESULT OpenLevel(_uint iLevelIndex, class CLevel* pNewLevel);
	HRESULT RenderLevel();
	_uint	GetCurLevelIdx();

public: /* For.Object_Manager */
	class	CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	HRESULT Add_Prototype(const wstring& pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clone_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, void* pArg = nullptr);
	void	Imgui_ProtoViewer(_uint iLevel, const _tchar*& szSelectedProto);
	void	Imgui_ObjectViewer(_uint iLevel, CGameObject*& pSelectedObject);
	class	CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);
	class	CGameObject* Find_GameObject(_uint iLevelIndex, const wstring & pLayerTag, const wstring & strObjName);
	void	SaveData(_uint iLevel,const wstring& strDirectory);
	void	LoadData(_uint iLevel, const wstring& strDirectory);
	class   CLayer*	Find_Layer(_uint iLevelIndex, const wstring& pLayerTag);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void* pArg = nullptr);
	void	Imgui_ComponentViewer(_uint iLevel, const _tchar*& szSelectedProto);
	void	Remove_ProtoComponent(_uint iLevel, const wstring & pComponentName);

public: // for imgui manager
	void Render_ImGui();
	void Render_Update_ImGui();
	void Add_ImguiObject(class CImguiObject* pImguiObject);
	void Clear_ImguiObjects();

public: /* For.PipeLine */
	_matrix		Get_TransformMatrix(CPipeLine::TRANSFORMSTATE eState);
	_float4x4	Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE eState);
	_float4x4	Get_TransformMatrix_Inverse(CPipeLine::TRANSFORMSTATE eState);
	void		Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4		Get_CamPosition();

public: /* For.Timer_Manager */
	_double		Get_TimeDelta(const wstring& pTimerTag);
	HRESULT		Ready_Timer(const wstring&  pTimerTag);
	void		Update_Timer(const wstring&  pTimerTag);
	class CTimer* Find_Timer(const wstring& pTimerTag);

public: /* For.Font_Manager */
	HRESULT Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& pFontTag, const wstring& pFontFilePath);
	HRESULT Render_Font(const wstring& pFontTag, const wstring& pText, _fvector vPos, _fvector vColor);

public: /* For. Light_Manager*/
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);

private:
	static _uint					m_iStaticLevelIndex;

private:
	class CGraphic_Device*		m_pGraphicDev = nullptr;
	class CInput_Device*		m_pInputDev = nullptr;
	class CLevel_Manager*		m_pLevelMgr = nullptr;
	class CObject_Manager*		m_pObjectMgr = nullptr;
	class CComponent_Manager*	m_pComponetMgr = nullptr;
	class CImGui_Manager*		m_pImGuiMgr = nullptr;
	CPipeLine*					m_pPipeLine = nullptr;
	class CTimer_Manager*		m_pTimerMgr = nullptr;
	class CFont_Manager*		m_pFontMgr = nullptr;
	class CLight_Manager*		m_pLightMgr = nullptr;

public: // Release_Engine
	static void Release_Engine();

private:
	HWND m_hWnd;

public:
	virtual void Free() override;
};

END