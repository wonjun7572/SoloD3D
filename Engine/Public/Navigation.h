#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CNavigation final : public CComponent
{
public:
	typedef struct tagNavigationDesc
	{
		_int iCurrentIndex = -1;
	}NAVIDESC;

private:
	CNavigation(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNavigation(const CNavigation& rhs);
	virtual ~CNavigation() = default;

public:
	virtual HRESULT Init_Prototype(const _tchar* pNavigationDataFilePath);
	virtual HRESULT Init(void* pArg) override;

public:
	_bool isMove_OnNavigation(_fvector TargetPos);
	
	HRESULT Update(const wstring pNavigationDataFilePath);

#ifdef _DEBUG
public:
	HRESULT Render();
#endif

private:
	vector<class CCell*>	m_Cells;
	NAVIDESC				m_NaviDesc;

private:
	HRESULT Ready_Neighbor();

#ifdef _DEBUG
private:
	class CShader*			m_pShader = nullptr;
#endif

public:
	static CNavigation* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pNavigationDataFilePath);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END