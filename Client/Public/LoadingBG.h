#pragma once

#include "Client_Define.h"
#include "UI.h"

BEGIN(Client)

class CLoadingBG final : public CUI
{
private:
	CLoadingBG(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLoadingBG(const CLoadingBG& rhs);
	virtual	~CLoadingBG() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	_uint m_iIndex = 0;

public:
	static CLoadingBG* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END