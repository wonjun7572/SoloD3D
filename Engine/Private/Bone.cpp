#include "..\Public\Bone.h"
#include "MathUtils.h"

CBone::CBone()
{
}

HRESULT CBone::Initialize(aiNode * pAINode, CBone* pParent)
{
	strcpy_s(m_szName, pAINode->mName.data);
	XMStoreFloat4x4(&m_OffsetMatrix, XMMatrixIdentity());
	memcpy(&m_TransformMatrix, &pAINode->mTransformation, sizeof(_float4x4));
	m_TransformMatrix = CMathUtils::Transpose(m_TransformMatrix);
	XMStoreFloat4x4(&m_CombindTransformMatrix, XMMatrixIdentity());
	m_pParent = pParent;
	Safe_AddRef(m_pParent);
	return S_OK;
}

void CBone::Compute_CombindTransformationMatrix()
{
	if (m_pParent == nullptr)
		m_CombindTransformMatrix = m_TransformMatrix;
	else
		m_CombindTransformMatrix = CMathUtils::Mul_Matrix(m_TransformMatrix, m_pParent->m_CombindTransformMatrix);
}

CBone * CBone::Create(aiNode * pAINode, CBone* pParent)
{
	CBone* pInstance = new CBone();

	if (FAILED(pInstance->Initialize(pAINode, pParent)))
	{
		MSG_BOX("Failed to Create : CBone");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CBone::Free()
{
	Safe_Release(m_pParent);
}