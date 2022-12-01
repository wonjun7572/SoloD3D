#pragma once

#include "Base.h"

BEGIN(Engine)

class CBone final : public CBase
{
private:
	CBone();
	virtual ~CBone() = default;

public:
	const char* Get_Name() const {	return m_szName;}
	void Set_OffsetMatrix(_float4x4 OffsetMatrix) {	m_OffsetMatrix = OffsetMatrix;}

public:
	HRESULT Initialize(aiNode* pAINode);
	void	Compute_CombindTransformationMatrix();

private:
	char				m_szName[MAX_PATH];
	_float4x4			m_OffsetMatrix;
	_float4x4			m_TransformMatrix;
	_float4x4			m_CombindTransformMatrix;
	CBone*				m_pParent;

public:
	static CBone* Create(aiNode* pAINode);
	virtual void Free() override;
};

END