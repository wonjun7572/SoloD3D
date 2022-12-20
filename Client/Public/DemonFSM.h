#pragma once

#include "Client_Define.h"
#include "FSMComponent.h"

BEGIN(Client)

class CDemonFSM : public CFSMComponent
{
public:
	virtual HRESULT Init(void* pArg) override;

	void Idle_Tick(_double TimeDelta);

	void Walk_Tick(_double TimeDelta);

	void Run_Tick(_double TimeDelta);

	void KnockDown_Ing(_double TimeDelta);

	void Hit_Ing(_double TimeDelta);

	void Attack_Ing(_double TimeDelta);

	_double m_KnockDownTime = 0.0;

	class CDemon* m_pTarget = nullptr;
	static CDemonFSM* Create(class CDemon* pTarget);
};

END