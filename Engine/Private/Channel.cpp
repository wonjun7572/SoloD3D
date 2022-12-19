#include "..\Public\Channel.h"
#include "Model.h"
#include "Bone.h"
#include "MathUtils.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(CHANNELLOAD& pAIChannel, CModel * pModel)
{
	strcpy_s(m_szName, pAIChannel.mNodeName);

	m_pBone = pModel->Get_BonePtr(m_szName);
	Safe_AddRef(m_pBone);

	m_iNumKeyframes = pAIChannel.mNumKeyFrames;

	m_KeyFrames = pAIChannel.mKeyFrames;

	return S_OK;
}

void CChannel::Update_TransformMatrix(_double PlayTime)
{
	_float3 vScale, vPosition;
	_float4	vRotation;
	_float4x4 TransformMatrix;

	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = m_KeyFrames.back().vScale;
		vRotation = m_KeyFrames.back().vRotation;
		vPosition = m_KeyFrames.back().vPosition;
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
			++m_iCurrentKeyFrameIndex;

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

		vScale = CMathUtils::Lerp(m_KeyFrames[m_iCurrentKeyFrameIndex].vScale, m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale, static_cast<float>(Ratio));
		vRotation = CMathUtils::QuaternionSlerp(m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation, m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation, static_cast<float>(Ratio));
		vPosition = CMathUtils::Lerp(m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition, m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition, static_cast<float>(Ratio));
	}

	TransformMatrix = CMathUtils::MatrixAffineTransformation(_float4(vScale.x, vScale.y, vScale.z, 0.f), _float4(0.f, 0.f, 0.f, 1.f), vRotation, _float4(vPosition.x, vPosition.y, vPosition.z, 1.f));

	m_pBone->Set_TransformMatrix(TransformMatrix);
}

_bool CChannel::Lerp_TransformMatrix(_double PlayTime, CChannel * Sour, CChannel * Dest, _bool bFinish)
{
	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;
	_matrix		Transform_Matrix;

	if (-1 == m_iLerpFrameIndex)
	{
		while (PlayTime >= Sour->m_KeyFrames[m_iLerpFrameIndex + 1].Time)
		{
			m_iLerpFrameIndex++;

			if ((_int)m_iNumKeyframes <= m_iLerpFrameIndex + 1)
			{
				m_iLerpFrameIndex -= 1;
				break;
			}
		}
	}

	_uint iSour;
	if (bFinish)	iSour = m_iNumKeyframes - 1;
	else			iSour = m_iLerpFrameIndex;

	_uint iDest = 0;

	if (Sour->m_KeyFrames.size() <= iSour)
		iSour = (_uint)Sour->m_KeyFrames.size() - 1;

	m_LerpRatio += 0.1f;

	_vector	vSourScale, vDestScale;
	_vector	vSourRotation, vDestRotation;
	_vector vSourPosition, vDestPosition;

	vSourScale = XMLoadFloat3(&Sour->m_KeyFrames[iSour].vScale);
	vDestScale = XMLoadFloat3(&Dest->m_KeyFrames[iDest].vScale);

	vSourRotation = XMLoadFloat4(&Sour->m_KeyFrames[iSour].vRotation);
	vDestRotation = XMLoadFloat4(&Dest->m_KeyFrames[iDest].vRotation);

	vSourPosition = XMLoadFloat3(&Sour->m_KeyFrames[iSour].vPosition);
	vDestPosition = XMLoadFloat3(&Dest->m_KeyFrames[iDest].vPosition);

	vScale = XMVectorLerp(vSourScale, vDestScale, static_cast<_float>(m_LerpRatio));
	vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, static_cast<_float>(m_LerpRatio));
	vPosition = XMVectorLerp(vSourPosition, vDestPosition, static_cast<_float>(m_LerpRatio));
	vPosition = XMVectorSetW(vPosition, 1.f);

	Transform_Matrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_TransformMatrix(Transform_Matrix);

	if (1.f <= m_LerpRatio)
	{
		m_LerpRatio = 0.f;
		return true;
	}

	return false;
}

CChannel * CChannel::Create(CHANNELLOAD& pAIChannel, CModel * pModel)
{
	CChannel*		pInstance = new CChannel();

	if (FAILED(pInstance->Initialize(pAIChannel, pModel)))
	{
		MSG_BOX("Failed to Created : CChannel");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CChannel::Free()
{
	Safe_Release(m_pBone);

	m_KeyFrames.clear();
}