#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)
// 이것을 트랜스폼처럼 바꿔볼 예정.
class ENGINE_DLL CVIBuffer_Point_Instancing final : public CVIBuffer_Instancing
{
public:
	CVIBuffer_Point_Instancing(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Instancing(const CVIBuffer_Point_Instancing& rhs);
	virtual ~CVIBuffer_Point_Instancing() = default;

public:
	virtual HRESULT Init_Prototype(_uint iNumInstance);
	virtual HRESULT Init(void* pArg) override;
	virtual HRESULT Tick(_double TimeDelta) override;
	HRESULT Go_Dir(_float4 vDir,  _double TimeDelta, _double dRespawnTime = 3.0 );

	virtual HRESULT Render();

private:
	_float*					m_pSpeeds = nullptr;
	_double					m_TimeDelta = 0.0;

public:
	static CVIBuffer_Point_Instancing* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END