#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Trail final : public CVIBuffer
{
protected:
	CVIBuffer_Trail(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Trail(const CVIBuffer_Trail& rhs);
	virtual ~CVIBuffer_Trail() = default;

public:
	virtual HRESULT Init_Prototype(const _uint& iCntX, const _uint& iCntZ);
	virtual HRESULT Init(void* pArg);

public:
	void	Copy_Indices(FACEINDICES32* pIndices, const _uint& iTriCnt);
	void	Get_VtxInfo(void* pVtxInfo);
	void	Set_VtxInfo(void* pVtxInfo);

private:
	_float3*	m_pPos;
	_uint		m_iNumVerticesX , m_iNumVerticesZ;

public:
	static CVIBuffer_Trail* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _uint& iCntX, const _uint& iCntZ);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END