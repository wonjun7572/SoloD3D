#pragma once

#include "Client_Define.h"
#include "UI.h"

BEGIN(Client)

class CSkillChargingUI final : public CUI
{
public:
	typedef struct tagSkillIcon
	{
		_float	fAlpha;
		_float  fAmount;
		_uint	iTexNum;

		_float  fSizeX;
		_float  fSizeY;
		_float  fX;
		_float  fY;

		_uint   iPassIndex;

	}SKILLICONDESC;

private:
	CSkillChargingUI(ID3D11Device* pDeviec, ID3D11DeviceContext* pContext);
	CSkillChargingUI(const CSkillChargingUI& rhs);
	virtual	~CSkillChargingUI() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty() override;

	void	Set_Amount(_float fAmount);
	_float  Get_Amount() { return m_SkillIconDesc.fAmount; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	SKILLICONDESC	m_SkillIconDesc;

public:
	static CSkillChargingUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END