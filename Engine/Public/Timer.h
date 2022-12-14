#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL  CTimer final :	public CBase
{
public:
	CTimer();
	virtual ~CTimer() = default;

public:
	_double		Get_TimeDelta(void) {return m_TimeDelta; }

public:
	HRESULT		Ready_Timer(void);
	void		Update_Timer(void);
	void		Set_SlowTimer(_float time) { m_fSlowTime = time; }

private:
	LARGE_INTEGER		m_CurrentTime;
	LARGE_INTEGER		m_OldTime;
	LARGE_INTEGER		m_OriginTime;
	LARGE_INTEGER		m_CpuTick;

	_double				m_TimeDelta = 0.0;

	_float				m_fSlowTime = 1.f;

public:
	static CTimer*	Create(void);
	virtual void Free(void);
};

END