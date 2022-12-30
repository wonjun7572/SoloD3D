#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CCell final : public CBase
{
public:
	enum POINT { POINT_A, POINT_B, POINT_C, POINT_END };
	enum NEIGHBOR { NEIGHBOR_AB, NEIGHBOR_BC, NEIGHBOR_CA, NEIGHBOR_END };

private:
	CCell(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CCell() = default;

public:
	_float3 Get_Point(POINT ePoint) { return m_vPoints[ePoint]; }

	void	Set_Neighbor(NEIGHBOR eNeighbor, CCell* pNeighbor) { m_iNeighborIndices[eNeighbor] = pNeighbor->m_iIndex; }

public:
	HRESULT Init(const _float3* pPoints, _int iIndex);

public:
	_bool	Compare_Points(const _float3& SourPoint, const _float3& DestPoint);
	_bool	isIn(_fvector vTargetPos, OUT _float4* vOldPos, OUT _int* pNeighborIndex);

	_bool	isIn(_fvector vTargetPos);

#ifdef _DEBUG
public:
	HRESULT Render(class CShader* pShader);
#endif

private:
	ID3D11Device*				m_pDevice = nullptr;
	ID3D11DeviceContext*		m_pContext = nullptr;

private:
	_int				m_iIndex = 0;
	_float3				m_vPoints[POINT_END];
	_int				m_iNeighborIndices[NEIGHBOR_END];

#ifdef _DEBUG
private:
	class CVIBuffer_Cell*			m_pVIBuffer = nullptr;
#endif // _DEBUG

public:
	static CCell* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _float3* pPoints, _int iIndex);
	virtual void Free();
};

END