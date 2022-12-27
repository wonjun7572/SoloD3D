#pragma once

#include "ImguiObject.h"

BEGIN(Engine)

class ENGINE_DLL CImgui_MapEditor final : public CImguiObject
{
private:
	CImgui_MapEditor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);

public:
	virtual HRESULT Initialize(void* pArg) override;

	virtual void Imgui_RenderTab() override;

	static CImgui_MapEditor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg = nullptr);
	virtual void Free() override;

private:
	class CGameObject* m_pSelectedObject = nullptr;
	class CGameInstance* m_pGameInstance = nullptr;
};

END