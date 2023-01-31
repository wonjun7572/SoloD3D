#pragma once

#include "Client_Define.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Dynamic : public CCamera
{
private:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg)override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render() override;

	void	Level_Chap1Tick(_double TimeDelta);
	void	Level_Chap2Tick(_double TimeDelta);
	void	Level_Chap3Tick(_double TimeDelta);
	
	void	Input_KeyBoard(_double TimeDelta);
	void	Mouse_Fix();

	void	Imgui_RenderProperty() override;
	
	HRESULT	Save_CheckPoints();
	HRESULT	Load_CheckPoints();

private:
	CGameObject*			m_pPlayerCam = nullptr;
	_bool					m_bCheckPointFinish = false;
	_double					m_TimeDelta = 0.0;
	vector<_float4>			m_CheckPoints;

	_float					m_fMultipleTime = 0.1f;

	_int					m_iIndex = 1;
	_bool					m_bFix = false;
	_bool					m_bPaused = false;

	_bool					m_bDynamicCam = true;

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END