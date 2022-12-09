#pragma once
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CBone;
END

class CRifle :	public CWeapon
{
private:
	CRifle(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CRifle(const CRifle& rhs);
	virtual ~CRifle() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty();

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	CGameObject* m_pPlayer = nullptr;
	CBone* m_pPlayerBone = nullptr;

	_float		m_X = 0.f, m_Y = 0.f, m_Z = 0.f;
	_float		m_RX = 0.f, m_RY = 0.f, m_RZ = 0.f;
	_float		m_SX = 0.f, m_SY = 0.f, m_SZ = 0.f;

public:
	static CRifle* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

