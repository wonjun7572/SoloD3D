#pragma once

#include "Client_Define.h"
#include "Camera.h"

BEGIN(Engine)
class CBone;
END

BEGIN(Client)

class CCamera_Dynamic : public CCamera
{
	enum TYPE {TYPE_DYNAMIC, TYPE_STATIC, TYPE_END };

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
	
	void	SwitchCam(_double TimeDelta);

	virtual void Imgui_RenderProperty();

private:
	void Mouse_Fix();
	bool m_bFix = false;
	_float m_fSensitivity = 0.f;

	CBone*		 m_pPlayerBone = nullptr;
	CGameObject* m_pPlayer = nullptr;
	
	_float		m_X = 0.f, m_Y = 0.f, m_Z = 0.f;
	_float		m_RX = 0.f, m_RY = 0.f, m_RZ = 0.f;
	_float		m_SX = 0.f, m_SY = 0.f, m_SZ = 0.f;

	TYPE		m_eType = TYPE_END;

	_bool		m_bStatic = false;

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END