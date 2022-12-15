#pragma once

#include "Character.h"

BEGIN(Client)

class CMidori final : public	CCharacter
{
private:
	CMidori(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CMidori(const CMidori& rhs);
	virtual ~CMidori() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void Movement(_double TimeDelta);

private:
	void	Collision_ToObstacle(_double TimeDelta);
	void	Collision_ToEnemy(_double TimeDelta);

private:
	void Imgui_RenderProperty() override;

private:
	class CMidoriFSM*			m_pFSM = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CMidori* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END