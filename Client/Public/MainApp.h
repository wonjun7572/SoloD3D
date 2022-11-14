#pragma once

#include "Base.h"
#include "Client_Define.h"
#include "TestTool.h"

BEGIN(Engine)
class CRenderer;
class CGameInstance;
END

BEGIN(Client)

class CMainApp final : public CBase
{
private:
	CMainApp();
	virtual ~CMainApp() = default;

public:
	HRESULT Init();
	void	Tick(_double deltaTime);
	HRESULT Render();

	HRESULT	Resize_BackBuffer();

private:
	CGameInstance* m_pGameInstance = nullptr;

	CRenderer*	   m_pRenderer = nullptr;

private:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

private:
	CTestTool* pTool = nullptr;

public:
	HRESULT Start_Level(LEVEL eLevelID);
	HRESULT Ready_Prototype_Component();
	HRESULT Ready_Prototype_GameObject();
	
public:
	static CMainApp* Create();
	virtual void Free() override;
};

END