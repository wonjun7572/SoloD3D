#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Cylinder final : public CVIBuffer
{
protected:
	CVIBuffer_Cylinder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Cylinder(const CVIBuffer_Cylinder& rhs);
	virtual ~CVIBuffer_Cylinder() = default;

public:
	virtual HRESULT Init_Prototype(_uint _segment);
	virtual HRESULT Init(void* pArg);

public:
	static CVIBuffer_Cylinder* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint _segment);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END
