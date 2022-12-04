#pragma once

#include "Client_Define.h"
#include "GameObject.h"
#include "DebugDraw.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CFSMComponent;
END

BEGIN(Client)

class CSheila final : public CGameObject
{
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

	void	Idle_OnStart();
	void	Idle_Tick(_double TimeDelta);
	void	Idle_OnExit();

	bool	IdleToWalk();
	
	void	Walk_OnStart();
	void	Walk_Tick(_double TimeDelta);
	void	Walk_OnExit();

	bool	WalkToIdle();
	bool	WalkToDash();
	bool	WalkToRun();

	void	Run_OnStart();
	void	Run_Tick(_double TimeDelta);
	void	Run_OnExit();

	bool	RunToIdle();

	void	Dash_OnStart();
	void	Dash_Tick(_double TimeDelta);
	void	Dash_OnExit();

	bool	DashToWalk();

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
	CFSMComponent*			m_pFSMCom = nullptr;

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