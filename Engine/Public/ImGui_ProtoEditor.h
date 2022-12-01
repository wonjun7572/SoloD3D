#pragma once

#include "ImGuiObject.h"

BEGIN(Engine)

class ENGINE_DLL CImGui_ProtoEditor final : public CImguiObject
{
private:
	CImGui_ProtoEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderTab() override;

	static CImGui_ProtoEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;

private:
	class CGameObject* m_pSelectedObject = nullptr;
	class CGameInstance* m_pGameInstance = nullptr;
};

END