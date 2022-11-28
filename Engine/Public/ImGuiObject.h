#pragma once
#include "Base.h"

BEGIN(Engine)

// imgui코드를 작성하고 CImgui_Manager로 관리하기 위한 부모클래스
class ENGINE_DLL CImguiObject abstract : public CBase
{
protected:
	CImguiObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg = nullptr);

	// imgui 렌더함수 원하는 형태로 구현, ImGui::Begin 부터 ImGui::End 까지 구현해야합니다.
	virtual void Imgui_FreeRender() {}

	// imgui manager의 tab에 추가할 기능 구현
	virtual void Imgui_RenderTab() {}

	// imgui manager의 window에 추가할 기능 구현
	virtual void Imgui_RenderWindow() {}

	/*
	*  imgui manager의 menu bar에 넣고 싶은 기능 구현
	*
	*	ex) 메뉴 추가 예제
	* 	if (ImGui::BeginMenu("MyMenu1"))
	*	{
	*		if (ImGui::MenuItem("Menu Button"))
	*		{
	*			// 클릭시 실행 코드
	*		}
	*		....
	*		ImGui::EndMenu();
	*	}
	*	.....
	*
	*/
	virtual void Imgui_RenderMenu() {}

	void Imgui_StartWindow();
	void Imgui_Menu_OpenWindow();

	const char* GetTabName() const { return m_szTabname; }
	const char* GetWindowName() const { return m_szWindowName; }
	_bool IsWindowOpened() const { return m_bOpenWindow; }

	virtual void Free() override;

protected:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

	// 탭과 윈도우 이름을 초기화해야 사용가능
	const char* m_szTabname = "##";
	const char* m_szWindowName = "##";

	_bool m_bOpenWindow = false;
	ImGuiWindowFlags m_ImguiWindowFlags = 0;
};

END