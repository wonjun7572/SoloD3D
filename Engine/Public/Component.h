#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CComponent abstract : public CBase
{
protected:
	CComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComponent(const CComponent& rhs);
	virtual ~CComponent() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);

public: /* imgui */
		// 이 컴포넌트에서 보여줄 데이터를 imgui로 작성한다.
	virtual void Imgui_RenderProperty() {}

protected:
	ID3D11Device* m_pDevice = nullptr;
	ID3D11DeviceContext* m_pContext = nullptr;

protected:
	_bool		  m_bClone;

public:
	virtual CComponent* Clone(void* Arg = nullptr) = 0;
	virtual void Free() override;
};

END