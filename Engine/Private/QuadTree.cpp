#include "..\public\QuadTree.h"
#include "Frustum.h"
#include "PipeLine.h"

CQuadTree::CQuadTree()
{
}

HRESULT CQuadTree::Initialize(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	m_iCorner[CORNER_LT] = iLT;
	m_iCorner[CORNER_RT] = iRT;
	m_iCorner[CORNER_RB] = iRB;
	m_iCorner[CORNER_LB] = iLB;

	if (1 == iRT - iLT)
		return S_OK;

	m_iCenterIndex = (iLT + iRB) >> 1;

	_uint			iLC, iTC, iRC, iBC;

	iLC = (iLT + iLB) >> 1;
	iTC = (iLT + iRT) >> 1;
	iRC = (iRT + iRB) >> 1;
	iBC = (iLB + iRB) >> 1;

	m_pChild[CHILD_LT] = CQuadTree::Create(iLT, iTC, m_iCenterIndex, iLC);
	m_pChild[CHILD_RT] = CQuadTree::Create(iTC, iRT, iRC, m_iCenterIndex);
	m_pChild[CHILD_RB] = CQuadTree::Create(m_iCenterIndex, iRC, iRB, iBC);
	m_pChild[CHILD_LB] = CQuadTree::Create(iLC, m_iCenterIndex, iBC, iLB);

	return S_OK;
}

void CQuadTree::Culling(CFrustum* pFrustum, const _float3 * pVerticesPos, FACEINDICES32* pIndices, _uint* pNumFaces)
{
	if (nullptr == m_pChild[CHILD_LT] ||
		true == LevelofDetail(pVerticesPos))
	{
		_bool			isIn[CORNER_END] = {
			pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]])),
			pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RT]])),
			pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RB]])),
			pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LB]])),
		};

		if (true == isIn[CORNER_LT] ||
			true == isIn[CORNER_RT] ||
			true == isIn[CORNER_RB])
		{
			pIndices[*pNumFaces]._0 = m_iCorner[CORNER_LT];
			pIndices[*pNumFaces]._1 = m_iCorner[CORNER_RT];
			pIndices[*pNumFaces]._2 = m_iCorner[CORNER_RB];
			++*pNumFaces;
		}

		if (true == isIn[CORNER_LT] ||
			true == isIn[CORNER_RB] ||
			true == isIn[CORNER_LB])
		{
			pIndices[*pNumFaces]._0 = m_iCorner[CORNER_LT];
			pIndices[*pNumFaces]._1 = m_iCorner[CORNER_RB];
			pIndices[*pNumFaces]._2 = m_iCorner[CORNER_LB];
			++*pNumFaces;
		}

		return;
	}

	_float		fRange = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LT]]) - XMLoadFloat3(&pVerticesPos[m_iCenterIndex])));

	if (true == pFrustum->isInFrustum_LocalSpace(XMLoadFloat3(&pVerticesPos[m_iCenterIndex]), fRange))
	{
		for (_uint i = 0; i < CHILD_END; ++i)
		{
			if (nullptr != m_pChild[i])
				m_pChild[i]->Culling(pFrustum, pVerticesPos, pIndices, pNumFaces);
		}
	}
	return;
}

_bool CQuadTree::LevelofDetail(const _float3 * pVerticesPos)
{
	CPipeLine*			pPipeLine = GET_INSTANCE(CPipeLine);

	_float4				vCamPosition = pPipeLine->Get_CamPosition();

	_float				fCamDistance = XMVectorGetX(XMVector3Length(XMLoadFloat4(&vCamPosition) - XMLoadFloat3(&pVerticesPos[m_iCenterIndex])));
	_float				fWidth = XMVectorGetX(XMVector3Length(XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_RB]]) - XMLoadFloat3(&pVerticesPos[m_iCorner[CORNER_LB]])));

	RELEASE_INSTANCE(CPipeLine);

	if (fCamDistance * 0.2f > fWidth)
		return true;

	return false;
}

CQuadTree * CQuadTree::Create(_uint iLT, _uint iRT, _uint iRB, _uint iLB)
{
	CQuadTree*		pInstance = new CQuadTree();

	if (FAILED(pInstance->Initialize(iLT, iRT, iRB, iLB)))
	{
		MSG_BOX("Failed to Created : CQuadTree");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CQuadTree::Free()
{
	for (auto& pChild : m_pChild)
		Safe_Release(pChild);
}
