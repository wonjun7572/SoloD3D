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
	
private:
	void Mouse_Fix();
	bool m_bFix = false;
	_float m_fSensitivity = 0.f;

	_bool m_bStatic = true;

	_float m_fCameraLatitude;
	_float m_fCameraLongitude;
	_float m_fOrbitDistance = 5.f;


public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END