#pragma once

#include "Client_Define.h"
#include "Base.h"

BEGIN(Client)

class CLoader final : public CBase
{
private:
	CLoader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLoader() = default;

public:
	HRESULT Init(LEVEL eNextLevelID);

	HRESULT Loading_ForLogo();
	HRESULT Loading_ForChapter_1();
	HRESULT Loading_ForChapter_2();
	HRESULT Loading_ForTool();

	void SwitchForThread(LEVEL eNextLevelID);

	LEVEL Get_NextLevelID() const { return m_eNextLevelID; }
	_bool isFinished() const { return m_isFinished; }
	wstring Get_LoadingText() const { return m_strLoadingText; }
	CRITICAL_SECTION Get_CriticalSection() const { return m_CriticalSection; }

private:
	ID3D11Device*		 m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

	CRITICAL_SECTION	 m_CriticalSection;

private:
	LEVEL	m_eNextLevelID = LEVEL_END;
	HANDLE	m_hThread;
	_bool	m_isFinished = false;
	wstring m_strLoadingText;

public:
	static CLoader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;
};

END