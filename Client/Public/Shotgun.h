#pragma once
#include "Weapon.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CBone;
END

class CShotgun :	public CWeapon
{
private:
	CShotgun(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShotgun(const CShotgun& rhs);
	virtual ~CShotgun() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	CGameObject* m_pPlayer = nullptr;
	CBone* m_pPlayerBone = nullptr;

public:
	static CShotgun* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

