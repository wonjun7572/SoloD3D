#pragma once

#include "Client_Define.h"
#include "FSMComponent.h"

BEGIN(Client)

class CMidoriFSM : public CFSMComponent
{
public:
	virtual HRESULT Init(void* pArg) override;

	void Idle_Tick(_double TimeDelta);

	void Run_Tick(_double TimeDelta);
	void Run_End();

	void Jump_Ing(_double TimeDelta);

	void Attack_Start();
	void Attack_Ing(_double TimeDelta);
	void Attack_End();

	void ExSkill_Ing(_double TimeDelta);
	void ExSkill_End();

	class CMidori* m_pTarget = nullptr;
	static CMidoriFSM* Create(class CMidori* pTarget);
};

END