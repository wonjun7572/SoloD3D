#pragma once

#include "Client_Define.h"
#include "UI.h"

BEGIN(Client)

class CLoadingUI final : public CUI
{
private:
	CLoadingUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadingUI(const CLoadingUI& rhs);
	virtual	~CLoadingUI() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CLoadingUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END