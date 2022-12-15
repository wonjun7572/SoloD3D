#pragma once

#include "Client_Define.h"
#include "FSMComponent.h"

BEGIN(Client)

class CTechSoldierFSM : public CFSMComponent
{
public:
	virtual HRESULT Init(void* pArg) override;

	void	Apper_Start();

	void	Idle_Tick(_double TimeDelta);

	void	Move_Tick(_double TimeDelta);
	void	Move_End();

	void	Attack_Start();
	void	Attack_Tick(_double TimeDelta);

	void	Death_Tick(_double TimeDelta);

	class	CTechSoldier* m_pTarget = nullptr;
	static	CTechSoldierFSM* Create(class CTechSoldier* pTarget);
};

END