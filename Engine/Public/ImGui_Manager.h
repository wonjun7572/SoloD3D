#pragma once

#include "Base.h"
#include "Tool.h"

BEGIN(Engine)

class CImGui_Manager : public CBase
{
	DECLARE_SINGLETON(CImGui_Manager)

private:
	CImGui_Manager();
	virtual ~CImGui_Manager() = default;

public:
	HRESULT	Ready_ImGui(HWND hWnd, ID3D11Device* pGraphicDev, ID3D11DeviceContext* pDeviceContext);
	void		ImGui_NewFrame(_double dTimeDelta);
	void		ImGui_Render();
	void		ImGui_Render_Update();

public:
	HRESULT	Add_Tool(CTool* pTool);

public:
	virtual		void	Free() override;

private:
	_double					m_dTimeDelta;
	vector<CTool*>		m_vecTool;
};

END