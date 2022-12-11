#pragma once

#include "Client_Define.h"
#include "GameObject.h"

class CWeapon;

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CFSMComponent;
END

BEGIN(Client)

class CSheila final : public CGameObject
{
public:
	enum WEAPON_TYPE {TYPE_RIFLE, TYPE_SHOTGUN,  TYPE_PISTOL, TYPE_SNIPER, TYPE_END};

private:
	CSheila(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSheila(const CSheila& rhs);
	virtual ~CSheila() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty() override;

	WEAPON_TYPE		Get_WeaponType() { return m_WeaponType; }

	void FindWeapon();
	void SwitchWeapon();
	void	Movement(_double TimeDelta);

private:
	CShader*						m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CModel*						m_pModelCom = nullptr;
	CFSMComponent*		m_pFSMCom = nullptr;

private:
	WEAPON_TYPE				m_WeaponType;
	CWeapon*					m_WeaponArray[TYPE_END] = { nullptr, nullptr, nullptr, nullptr};

	_float							m_fSensitivity = 0.05f;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	_uint m_iCurrentAnimIndex = 0;

public:
	static CSheila* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END