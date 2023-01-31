#pragma once

#include "Client_Define.h"
#include "Camera.h"

BEGIN(Client)

class CLightCamera : public CCamera
{
private:
	CLightCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLightCamera(const CLightCamera& rhs);
	virtual ~CLightCamera() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg)override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render() override;

public:
	static CLightCamera* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END