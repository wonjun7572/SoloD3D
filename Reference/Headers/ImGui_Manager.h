#pragma once
#include "Base.h"

BEGIN(Engine)
class CImguiObject;

class CImGui_Manager final : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	void Ready_Imgui(HWND hWnd, ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	void Tick_Imgui();
	void Render_Imgui();
	void Render_Update_ImGui();

	// imgui object를 새로운 window로 추가한다.
	void Add_ImguiObject(CImguiObject* pImguiObject);

	// 현재 사용중인 imgui object를 모두 삭제한다.
	void Clear_ImguiObjects();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	vector<CImguiObject*> m_vecObj;

	ImGuiWindowFlags m_iWindowFlags = ImGuiWindowFlags_MenuBar;
	ImGuiTabBarFlags m_iTabBarFlags = ImGuiTabBarFlags_None;

public:
	virtual void Free() override;
};

END