#pragma once

#include "GameObject.h"
#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct tagCameraDesc : public CGameObject::GAMEOBJECTDESC
	{
		_float4				vEye;
		_float4				vAt;
		_float4				vUp;

		_float				fAspect;
		_float				fFovy;
		_float				fNear;
		_float				fFar;
	}CAMERADESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_double fTimeDelta);
	virtual void Late_Tick(_double fTimeDelta);
	virtual HRESULT Render();

	virtual void Imgui_RenderProperty() override;

protected:
	CAMERADESC						m_CameraDesc;
	class CPipeLine*				m_pPipeLine = nullptr;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END