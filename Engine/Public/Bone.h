#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
public:
	typedef struct BoneLoadTag
	{
		char mName[MAX_PATH];
		_float4x4 mTransformation;
	}BONELOAD;

private:
	CBone();
	virtual ~CBone() = default;

public:
	const char* Get_Name() const {	return m_szName;}
	void Set_OffsetMatrix(_float4x4 OffsetMatrix) {	m_OffsetMatrix = OffsetMatrix;}
	_float4x4 Get_OffsetMatrix() { return m_OffsetMatrix; }

	void Set_CombindTransformMatrix(_float4x4 CombindTransformMatrix) { m_CombindTransformMatrix = CombindTransformMatrix; }
	_float4x4 Get_CombindMatrix() { return m_CombindTransformMatrix; }

	void Set_TransformMatrix(_fmatrix TransformMatrix) { XMStoreFloat4x4(&m_TransformMatrix, TransformMatrix); }
	void Set_TransformMatrix(_float4x4 TransformMatrix) { m_TransformMatrix = TransformMatrix; }
	_float4x4 Get_TransformMatrix() { return m_TransformMatrix; }

	BONELOAD&		Get_BONELOAD() { return m_BONELOAD; }
	CBone*			Get_BoneParent() { return m_pParent; }
	
public:
	HRESULT Initialize(BONELOAD& BoneLoad, CBone* pParent);
	void	Compute_CombindTransformationMatrix();

private:
	char				m_szName[MAX_PATH];
	_float4x4			m_OffsetMatrix;
	_float4x4			m_TransformMatrix;
	_float4x4			m_CombindTransformMatrix;
	CBone*				m_pParent = nullptr;
	BONELOAD			m_BONELOAD;

public:
	static CBone* Create(BONELOAD& BoneLoad, CBone* pParent);
	virtual void Free() override;
};

END