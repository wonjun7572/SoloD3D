#pragma once

#include "VIBuffer.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Terrain final : public CVIBuffer
{
protected:
	CVIBuffer_Terrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Terrain(const CVIBuffer_Terrain& rhs);
	virtual ~CVIBuffer_Terrain() = default;

public:
	const _float3*		Get_VtxPos() const { return m_pPos; }
	_uint	Get_VtxCntX() const { return m_iNumVerticesX; }
	_uint	Get_VtxCntZ() const { return m_iNumVerticesZ; }

	virtual HRESULT Init_Prototype(const _tchar* pHeightMapFilePath);
	virtual HRESULT Init(void* pArg);

private:
	_uint			m_iNumVerticesX = 0;
	_uint			m_iNumVerticesZ = 0;

	_float3*			m_pPos;

public:
	static CVIBuffer_Terrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pHeightMapFilePath);
	virtual CComponent* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

END