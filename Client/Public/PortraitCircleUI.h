#pragma once

#include "Client_Define.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CPortraitCircleUI final : public CUI
{
private:
	CPortraitCircleUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPortraitCircleUI(const CPortraitCircleUI& rhs);
	virtual	~CPortraitCircleUI() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT	SetUp_ShaderResourcesIcon();
	HRESULT	SetUp_ShaderResourcesAni();
	_float	m_fAlpha = 1.f;
	_float	m_fFrame = 1.f;
	class CTexture* m_pPortraitCom = nullptr;

public:
	static CPortraitCircleUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END