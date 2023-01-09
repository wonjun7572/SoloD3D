#pragma once
#include "GameObject.h"

BEGIN(Engine)


class ENGINE_DLL CUI abstract : public CGameObject
{

protected:
	CUI(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Init_Prototype()override;
	virtual HRESULT Init(void* pArg)override;

	virtual void	Tick(_double TimeDelta)override;
	virtual void	Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;
	virtual void	Set_parentName(_uint iCulLevel, const _tchar* pParentTag);

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END