#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Client)

class CTree final : public CGameObject
{
public:
	CTree(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTree(const CTree& rhs);
	virtual ~CTree() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual	HRESULT	Init(void* pArg) override;
	virtual	void	Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void	UI_Tick(_double TimeDelta);

	void	Imgui_RenderProperty() override;

	_float Get_Hp() { return m_fHp; }

private:
	CGameObject* m_pUI = nullptr;
	CGameObject* m_pCountDown = nullptr;

	_float	m_fHp = 3000.f;
	_float	m_fMaxHp = 3000.f;

	wstring		m_strCountDown;

public:
	static CTree* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END