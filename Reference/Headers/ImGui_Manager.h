#pragma once

#include "Base.h"

BEGIN(Engine)

class CImguiObject;

class CImGui_Manager : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	void Ready_Imgui(HWND hWnd, ID3D11Device* pDeviceOut, ID3D11DeviceContext* pDeviceContextOut);
	void Tick_Imgui();
	void Render_Imgui();

	// imgui object�� tab�� �߰��Ѵ�.
	void Add_ImguiTabObject(CImguiObject* ImguiObject);

	// imgui object�� ���ο� window�� �߰��Ѵ�.
	void Add_ImguiWindowObject(CImguiObject* ImguiObject);

	// ���� ������� imgui object�� ��� �����Ѵ�.
	void Clear_ImguiObjects();

private:
	void RenderTab();
	void RenderWindow();

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pDeviceContext = nullptr;

	vector<CImguiObject*>	m_vecTab;
	vector<CImguiObject*>	m_vecWin;

public:
	virtual void Free() override;
};

END