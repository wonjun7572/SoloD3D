#pragma once

#include "Client_Define.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Dynamic : public CCamera
{
public:
	enum CAM_STATE { STATE_DYNAMIC, STATE_TPS, STATE_ORIGINAL, STATE_END };
	enum CAM_CHARACTER { CHARACTER_MOMOI, CHARACTER_IZUNA, CHARACTER_AZUSA,
		CHARACTER_MIDORI, CHARCTER_END };

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
	
	CAM_STATE Get_CAMSTATE() { return m_eState; }
	CAM_CHARACTER Get_CHARACTERCAM() { return m_eCharacter; }

private:
	void	Imgui_RenderProperty() override;

private:
	void Mouse_Fix();
	bool m_bFix = false;

	_float m_fSensitivity = 0.f;

	CAM_STATE		m_eState = STATE_ORIGINAL;
	CAM_CHARACTER   m_eCharacter = CHARCTER_END;

	_bool m_bFound = false;
	_bool m_bStatic = false;

	class CTransform* m_pCharacterTransform[CHARCTER_END] = { nullptr };

	_float m_X = 0, m_Y = 0, m_Z = 0;
	_float m_RX = 0, m_RY = 0, m_RZ = 0;

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END