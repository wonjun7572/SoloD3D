#pragma once

#include "Client_Define.h"
#include "Camera.h"

BEGIN(Client)

class CPlayerCamera : public CCamera
{
private:
	CPlayerCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CPlayerCamera(const CPlayerCamera& rhs);
	virtual ~CPlayerCamera() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg)override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render() override;

private:
	void	Imgui_RenderProperty() override;

private:
	void Mouse_Fix();
	
	bool m_bFix = true;

	CGameObject* m_pTarget = nullptr;

	_float4 m_vLookAt = _float4(0.f, 0.f, 0.f,-1.f);
	_float4 m_vPlayerPos;

	_float m_X = 0, m_Y = 0, m_Z = 0 , m_Y1 = 0;
	_float m_RX = 0, m_RY = 0, m_RZ = 0;

public:
	static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

