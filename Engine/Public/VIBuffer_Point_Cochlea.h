#pragma once

#include "VIBuffer_Instancing.h"

BEGIN(Engine)

class ENGINE_DLL CVIBuffer_Point_Cochlea final : public CVIBuffer_Instancing
{
public:
	typedef struct tagVIBufferCochelea
	{
		_uint		iNumInstances;
		_float	fRange;
		_float   fMinTime, fMaxTime;
		_float	fMinSpeed, fMaxSpeed;
		_float2 vMinSize, vMaxSize;
		_uint		iNumSprite;

		_uint		iNumSpreadInstances;
		_float	fMaxSpreadSizeOffset, fMinSpreadSizeOffset;
		_float	fRangeOffset;
	}DESC;

private:
	struct InstanceData
	{
		_float fTime, fFrame;
		_float2 vSize;
		_float fSpeed;
		_uint iIndex;
	};

	struct SpreadData
	{
		_float4 vDir;
		_float	fSpeed;
	};

private:
	InstanceData*		m_pInstanceData = nullptr;
	InstanceData*		m_pInstanceDataOriginal = nullptr;

	SpreadData*			m_pSpreadData = nullptr;

	DESC	m_Desc;

public:
	CVIBuffer_Point_Cochlea(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CVIBuffer_Point_Cochlea(const CVIBuffer_Point_Cochlea& rhs);
	virtual ~CVIBuffer_Point_Cochlea() = default;

public:
	virtual HRESULT Init_Prototype(_uint iNumInstance);
	virtual HRESULT Init(void* pArg) override;
	virtual HRESULT Tick(_double TimeDelta) override;
	virtual HRESULT Render();

	HRESULT    Go_Dir(_float4 vDir, _double TimeDelta);

	void	  Reset();

public:
	static CVIBuffer_Point_Cochlea* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iNumInstance = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END