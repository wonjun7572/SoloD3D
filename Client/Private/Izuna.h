#pragma once

#include "Character.h"

BEGIN(Client)

class CIzuna final : public CCharacter
{
private:
	CIzuna(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CIzuna(const CIzuna& rhs);
	virtual ~CIzuna() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	Collision_ToObstacle(_double TimeDelta);
	void	Collision_ToEnemy(_double TimeDelta);

private:
	void Movement(_double TimeDelta);

private:
	void Imgui_RenderProperty() override;
	_float3					m_ObstaclePos;

private:
	_double					m_MoveTime = 0;
	class CIzunaFSM*		m_pFSM = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CIzuna* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END