#pragma once

#include "Client_Define.h"
#include "UI.h"

BEGIN(Engine)
class CTexture;
END

BEGIN(Client)

class CBloodFrameUI final : public CUI
{
private:
	CBloodFrameUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBloodFrameUI(const CBloodFrameUI& rhs);
	virtual	~CBloodFrameUI() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty() override;

	void	Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	_float	m_fAlpha = 1.f;

public:
	static CBloodFrameUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END