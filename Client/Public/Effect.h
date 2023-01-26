#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CTexture;
END

BEGIN(Client)
class CEffect : public CGameObject
{
protected:
	CEffect(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CEffect(const CEffect& rhs);
	virtual	~CEffect() = default;

protected:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

public:
	void	Set_UV(_float2 UV) { m_UVMoveFactor = UV; }
	_float2	Get_UV() { return m_UVMoveFactor; }

	void	Set_Alpha(_float fAlpha) { m_fAlpha = fAlpha; }
	void	Set_UVSpeed(_float fUVSpeed) { m_fUVSpeed = fUVSpeed; }
	void	LinkObject(_double TimeDelta, _float4 targetpos);

protected:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;

	_float						m_fAlpha = 1.f;
	_float						m_fUVSpeed = 1.f;
	_float2						m_UVMoveFactor;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END