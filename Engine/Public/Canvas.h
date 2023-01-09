#pragma once
#include "UI.h"

BEGIN(Engine)

class ENGINE_DLL CCanvas abstract :public CUI
{
protected:
	CCanvas(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CCanvas(const CCanvas& rhs);
	virtual ~CCanvas() = default;

public:
	virtual HRESULT Init_Prototype()override;
	virtual HRESULT Init(void* pArg)override;
	virtual void Tick(_double TimeDelta)override;
	virtual void Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;

public:
	virtual HRESULT	Add_ChildUI(CUI* pUI)sealed;

protected:
	vector<CUI*>		m_ChildrenVec;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END