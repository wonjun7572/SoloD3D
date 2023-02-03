#pragma once

#include "Effect_Point.h"

class CRainEffect final : public Effect_Point
{
private:
    CRainEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    CRainEffect(const Effect_Point& rhs);
    virtual ~CRainEffect() = default;

public:
    virtual HRESULT Init_Prototype() override;
    virtual HRESULT Init(void* pArg) override;
    virtual void Tick(_double TimeDelta) override;
    virtual void Late_Tick(_double TimeDelta) override;
    virtual HRESULT Render() override;
  
private:
    HRESULT SetUp_Components();
    HRESULT SetUp_ShaderResources();
    CGameObject*	m_pPlayerCam = nullptr;
    CGameObject*    m_pDynamicCam = nullptr;

public:
    static CRainEffect* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    virtual CGameObject* Clone(void* pArg = nullptr);
    virtual void Free() override;
};
