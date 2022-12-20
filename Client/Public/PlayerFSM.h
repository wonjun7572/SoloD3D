#pragma once

#include "Client_Define.h"
#include "FSMComponent.h"

BEGIN(Client)

class CPlayerFSM : public CFSMComponent
{
public:
	virtual HRESULT Init(void* pArg) override;

	void Idle_Tick(_double TimeDelta);

	void Walk_Tick(_double TimeDelta);

	void Run_Tick(_double TimeDelta);
	void Run_End();

	void Jump_Ing(_double TimeDelta);

	void Attack_Ing(_double TimeDelta);
	void Attack1_Ing(_double TimeDelta);
	void Attack2_Ing(_double TimeDelta);
	
	class CPlayer* m_pTarget = nullptr;
	static CPlayerFSM* Create(class CPlayer* pTarget);
};

END