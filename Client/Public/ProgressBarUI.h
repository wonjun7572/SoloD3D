#pragma once

#include "Client_Define.h"
#include "UI.h"

BEGIN(Client)

class CProgressBarUI final : public CUI
{
public:
	typedef struct tagProgressBar
	{
		_float fAmount;

		_float  fSizeX;
		_float  fSizeY;
		_float  fX;
		_float  fY;

		_uint  iAlbedoTexNum;
		_uint  iFillTexNum;
		_uint  iBackTexNum;

		_int   iPassIndex;
	}PROGRESSBARDESC;

private:
	CProgressBarUI(ID3D11Device* pDeviec, ID3D11DeviceContext* pContext);
	CProgressBarUI(const CProgressBarUI& rhs);
	virtual	~CProgressBarUI() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty() override;

	void	Set_Amount(_float fAmount);
	_float  Get_Amount() { return m_ProgressBarDesc.fAmount; }

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	PROGRESSBARDESC	m_ProgressBarDesc;

public:
	static CProgressBarUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END