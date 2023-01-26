#pragma once

#include "Client_Define.h"
#include "Camera.h"

BEGIN(Engine)
class CTransform;
END

BEGIN(Client)

class CPlayer;

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

	void ShakeUpdate(_double TimeDelta);

	void Shake(_float fShakeDuration, _float fShakeAmount = 0.1f, _float fDecreaseFactor = 1.f);

	void LinkPlayer(_double TimeDelta, CTransform* pTarget, _bool bCamTurn);
	void DynamicCamera(_double TimeDelta);

	void Set_CinematicCam(_bool bCinematic) { m_bCinematic = bCinematic; }

private:
	void Mouse_Fix();
	
	bool m_bFix = true;

	_float m_vLookImgui = 1.f;

	CPlayer* m_pPlayer = nullptr;

	_float4 m_vLookAt = _float4(0.f, 0.f, 0.f,-1.f);
	_float4 m_vPlayerPos;

	_float m_fDistanceToTarget = 5.f;
	_float m_fCamY = 0.f;
	_float m_fCamTime = 0.f;

	_float m_fCamImguiY = 3.f;
	_float m_fCamImguiLookY = 3.f;

	_bool m_bChange = false;

	_float m_fShakeDuration = 0.f;
	_float m_fShakeAmount = 0.f;
	_float m_fDecreaseFactor = 0.f;


	// 시네마틱 카메라가 없다면 true해줘야함
	_bool  m_bCinematic = true;

public:
	static CPlayerCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

