#pragma once

#include "Base.h"
#include "Component_Manager.h"
#include "Tool.h"
#include "PipeLine.h"

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance);

private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: // GameInstance
	HRESULT		Init_Engine(_uint iNumLevels, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut);
	void		Tick_Engine(_double TimeDelta);
	void Clear_Level(_uint iLevelIndex);

public: // Graphic_Device
	HRESULT				 Clear_Graphic_Device(const _float4* pColor);
	HRESULT				 Present();
	HRESULT				 Update_SwapChain(HWND hWnd, _uint iWinCX, _uint iWinCY, _bool bIsFullScreen, _bool bNeedUpdate);

public:		/* For InputDevice */
	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(MOUSEKEYSTATE byMouseID);
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMoveState);
	_bool		Mouse_Down(MOUSEKEYSTATE MouseButton);
	_bool		Mouse_Up(MOUSEKEYSTATE MouseButton);
	_bool		Mouse_DoubleClick(MOUSEKEYSTATE MouseButton);
	_bool		Key_Down(_ubyte byKeyID);
	_bool		Key_Up(_ubyte byKeyID);
	void		Reset_EveryKey();

	
public: // Level_Manager
	HRESULT OpenLevel(_uint iLevelIndex, class CLevel* pNewLevel);
	HRESULT RenderLevel();

public: /* For.Object_Manager */
	HRESULT Add_Prototype(const wstring& pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clone_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& pPrototypeTag, void* pArg = nullptr);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void* pArg = nullptr);

public:		/* For ImGui_Manager */
	void		ImGui_Render();
	void		ImGui_Render_Update();
	HRESULT		Add_Tool(CTool* pTool);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_matrix Get_TransformMatrix(CPipeLine::TRANSFORMSTATE  eState);
	_float4x4 Get_TransformFloat4x4(CPipeLine::TRANSFORMSTATE  eState);
	_float4x4 Get_TransformFloat4x4_TP(CPipeLine::TRANSFORMSTATE  eState);
	_float4 Get_CamPosition();

private:
	class CGraphic_Device*		m_pGraphicDev = nullptr;
	class CInput_Device*		m_pInputDev = nullptr;
	class CLevel_Manager*		m_pLevelMgr = nullptr;
	class CObject_Manager*		m_pObjectMgr = nullptr;
	class CComponent_Manager*	m_pComponetMgr = nullptr;
	class CImGui_Manager*		m_pImGuiMgr = nullptr;
	CPipeLine*					m_pPipeLine = nullptr;

public: // Release_Engine
	static void Release_Engine();

public:
	virtual void Free() override;
};

END