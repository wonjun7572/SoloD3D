#include "..\public\Imgui_Manager.h"
#include "Graphic_Device.h"
#include "ImGui/ImGuiFileDialog.h"
#include "ImguiObject.h"

IMPLEMENT_SINGLETON(CImGui_Manager)

CImGui_Manager::CImGui_Manager()
{

}

void CImGui_Manager::Ready_Imgui(HWND hWnd, ID3D11Device* pDevice, ID3D11DeviceContext* pDeviceContextOut)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;		// Enable Gamepad Controls
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
	io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows
	//io.ConfigViewportsNoAutoMerge = true;
	io.ConfigViewportsNoTaskBarIcon = true;

	ImGui::StyleColorsDark();

	ImGuiStyle& style = ImGui::GetStyle();
	if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		style.WindowRounding = 0.0f;
		style.Colors[ImGuiCol_WindowBg].w = 1.0f;
	}

	m_pDevice = pDevice;
	m_pDeviceContext = pDeviceContextOut;
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pDeviceContext);

	ImGui_ImplWin32_Init(hWnd);
	ImGui_ImplDX11_Init(m_pDevice, m_pDeviceContext);
}

void CImGui_Manager::Tick_Imgui()
{
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
}

void CImGui_Manager::Render_Imgui()
{
	RenderTab();
	RenderWindow();

	ImGui::EndFrame();
	ImGui::Render();
}

void CImGui_Manager::Render_Update_ImGui()
{
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

	if (ImGui::GetIO().ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
	{
		ImGui::UpdatePlatformWindows();
		ImGui::RenderPlatformWindowsDefault();
	}
}

void CImGui_Manager::Add_ImguiTabObject(CImguiObject* ImguiObject)
{
	if (ImguiObject == nullptr)
		return;

	m_vecTab.push_back(ImguiObject);
}

void CImGui_Manager::Add_ImguiWindowObject(CImguiObject* ImguiObject)
{
	if (ImguiObject == nullptr)
		return;

	m_vecWin.push_back(ImguiObject);
}

void CImGui_Manager::Clear_ImguiObjects()
{
	for (auto& e : m_vecTab)
		Safe_Release(e);
	m_vecTab.clear();

	for (auto& e : m_vecWin)
		Safe_Release(e);
	m_vecWin.clear();
}

void CImGui_Manager::RenderTab()
{
	if (m_vecTab.empty())
		return;

	ImGui::Begin("TabWindow");
	if (ImGui::BeginTabBar("Manager_Tab", ImGuiTabBarFlags_None))
	{
		for (const auto& imTabObj : m_vecTab)
		{
			if (ImGui::BeginTabItem(imTabObj->GetTabName()))
			{
				imTabObj->Imgui_RenderTab();
				ImGui::EndTabItem();
			}
		}
		ImGui::EndTabBar();
	}
	ImGui::End();
}

void CImGui_Manager::RenderWindow()
{
	for (const auto& imWinObj : m_vecWin)
	{
		ImGui::Begin(imWinObj->GetWindowName());
		imWinObj->Imgui_RenderWindow();
		ImGui::End();
	}
}

void CImGui_Manager::Free()
{
	Clear_ImguiObjects();

	Safe_Release(m_pDevice);
	Safe_Release(m_pDeviceContext);
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}
