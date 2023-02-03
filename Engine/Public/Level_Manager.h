#pragma once

#include "Base.h"

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)

private:
	CLevel_Manager();
	virtual	~CLevel_Manager() = default;

public:
	HRESULT OpenLevel(_uint iLevelIndex, class CLevel* pNewLevel);
	void	Tick(_double TimeDelta);
	void	Late_Tick(_double TimeDelta);
	HRESULT Render();
	
	_uint GetCurLevelIdx() { return m_iLevelIndex; }

	void  Set_bOpenLevel(_bool bOpen) { m_bOpenLevel = bOpen; }
	_bool Get_bOpenLevel() {return m_bOpenLevel; }
	
private:
	class CLevel* m_pCurrentLevel = nullptr;
	_uint		  m_iLevelIndex = 0;

	_bool		  m_bOpenLevel = false;
	
public:
	virtual void Free() override;
};

END