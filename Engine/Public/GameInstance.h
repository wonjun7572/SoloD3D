#pragma once

#include "Base.h"
#include "Component_Manager.h"
#include "PipeLine.h"
#include "Sound.h"

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
	ID3D11Device*		 Get_Device();
	ID3D11DeviceContext* Get_Context();

public:		/* For InputDevice */
	_byte		Get_DIKeyState(_ubyte byKeyID);
	_byte		Get_DIMouseState(MOUSEKEYSTATE byMouseID);
	_long		Get_DIMouseMove(MOUSEMOVESTATE eMoveState);
	_bool		Mouse_Down(MOUSEKEYSTATE MouseButton, _bool* pData = nullptr);
	_bool		Mouse_Up(MOUSEKEYSTATE MouseButton, _bool* pData = nullptr);
	_bool		Key_Down(_ubyte byKeyID, _bool* pData = nullptr);
	_bool		Key_Up(_ubyte byKeyID, _bool* pData = nullptr);
	_bool		Key_Pressing(_ubyte byKeyID);

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
	const class	list<CGameObject*>* Find_LayerList(_uint iLevelIndex, const wstring& pLayerTag);
	void	SaveData(_uint iLevel,const wstring& strDirectory);
	void	LoadData(_uint iLevel, const wstring& strDirectory);
	void	SaveMapObjectData(_uint iLevel, const wstring& pLayerTag, const wstring& strDirectory);
	void	LoadMapObjectData(_uint iLevel, const wstring& pLayerTag, const wstring& strDirectory);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void* pArg = nullptr);
	void	Imgui_ComponentViewer(_uint iLevel, const _tchar*& szSelectedProto);
	void	Imgui_ModelComponetViewer(_uint iLevel, OUT const _tchar*& szSelectedProto);
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
	_float3		Get_CamUp();
	_float3		Get_CamLook();

public: /* For.Timer_Manager */
	_double		Get_TimeDelta(const wstring& pTimerTag);
	HRESULT		Ready_Timer(const wstring&  pTimerTag);
	void		Update_Timer(const wstring&  pTimerTag);
	class CTimer* Find_Timer(const wstring& pTimerTag);

public: /* For.Font_Manager */
	HRESULT Add_Fonts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& pFontTag, const wstring& pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor = XMVectorSet(1.f, 1.f, 1.f, 1.f));
	void	DrawTextInWorld(const _tchar* pFontTag, const _tchar* text, _float4 vPos, _float3 vScale, _float4 vColor);

public: /* For. Light_Manager*/
	const LIGHTDESC* Get_LightDesc(_uint iIndex);
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);
	void	Clear_Light();
	
public: /* For.Frustum */
	_bool isInFrustum_WorldSpace(_fvector vWorldPos, _float fRange = 0.f);
	_bool isInFrustum_LocalSpace(_fvector vLocalPos, _float fRange = 0.f);

public: /* For.Sound_Manager */
	void Play_Sound(const _tchar *pSoundKey, _float fVolume, _bool bIsBGM = false, _int iManualChannelIndex = -1);
	void Stop_Sound(_uint iManualChannelIndex);
	void Set_Volume(_uint iManualChannelIndex, _float fVolume);
	void Set_MasterVolume(_float fVolume);
	void Set_SoundDesc(const _tchar *pSoundKey, CSound::SOUND_DESC& SoundDesc);

public: /* For. Target_Manager */
	ID3D11ShaderResourceView* Get_DepthTargetSRV();

private:
	static _uint					m_iStaticLevelIndex;

private:
	class CGraphic_Device*			m_pGraphicDev = nullptr;
	class CInput_Device*			   m_pInputDev = nullptr;
	class CLevel_Manager*			m_pLevelMgr = nullptr;
	class CObject_Manager*		m_pObjectMgr = nullptr;
	class CComponent_Manager*	m_pComponetMgr = nullptr;
	class CImGui_Manager*		m_pImGuiMgr = nullptr;
	CPipeLine*					m_pPipeLine = nullptr;
	class CTimer_Manager*		m_pTimerMgr = nullptr;
	class CFont_Manager*		m_pFontMgr = nullptr;
	class CLight_Manager*		m_pLightMgr = nullptr;
	class CFrustum*				m_pFrustum = nullptr;
	class CSound_Manager*		m_pSoundMgr = nullptr;
	class CTarget_Manager*		m_pTargetMgr = nullptr;

public: // Release_Engine
	static void Release_Engine();

private:
	HWND m_hWnd;

public:
	virtual void Free() override;
};

END