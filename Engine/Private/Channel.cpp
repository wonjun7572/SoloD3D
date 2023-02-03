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

	m_pTrailBone = pModel->Get_BonePtr(m_szName);
	Safe_AddRef(m_pTrailBone);
	
	m_iNumKeyframes = pAIChannel.mNumKeyFrames;

	m_KeyFrames = pAIChannel.mKeyFrames;

	return S_OK;
}

void CChannel::Update_TransformMatrix(_double PlayTime)
{
	_vector		vScale;
	_vector		vRotation;
	_vector		vPosition;
	_matrix		Transform_Matrix;

	XMStoreFloat4(&m_vMovePos, XMVectorSet(0.f, 0.f, 0.f, 1.f));

	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
			m_iCurrentKeyFrameIndex++;

		_double	Ratio =
			(PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time)
			/ (m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

		_vector	vSourScale, vDestScale;
		_vector	vSourRotation, vDestRotation;
		_vector vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);

		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);

		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, static_cast<_float>(Ratio));
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, static_cast<_float>(Ratio));
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, static_cast<_float>(Ratio));
		vPosition = XMVectorSetW(vPosition, 1.f);

		if (!strcmp("Bip01", m_szName))
		{
			XMStoreFloat4(&m_vMovePos, vPosition);
			vPosition = XMVectorSet(0.f,0.f,XMVectorGetZ(vPosition),1.f);
		}
	}

	Transform_Matrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_TransformMatrix(Transform_Matrix);
	m_pTrailBone->Set_TransformMatrix(Transform_Matrix);
}

void CChannel::Blend_TransformMatrix(_double PlayTime, _float fBlendRatio)
{
	_vector vBaseScale, vBaseRot, vBasePos;
	XMMatrixDecompose(&vBaseScale, &vBaseRot, &vBasePos, XMLoadFloat4x4(&m_pBone->Get_TransformMatrix()));

	_vector			vScale;
	_vector			vRotation;
	_vector			vPosition;

	/* 현재 재생된 시간이 마지막 키프레임시간보다 커지며.ㄴ */
	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vScale = XMLoadFloat3(&m_KeyFrames.back().vScale);
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
		vPosition = XMLoadFloat3(&m_KeyFrames.back().vPosition);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
		{
			++m_iCurrentKeyFrameIndex;
		}

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

		_vector			vSourScale, vDestScale;
		_vector			vSourRotation, vDestRotation;
		_vector			vSourPosition, vDestPosition;

		vSourScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vScale);
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vSourPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition);

		vDestScale = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vDestPosition = XMLoadFloat3(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition);

		vScale = XMVectorLerp(vSourScale, vDestScale, (float)Ratio);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (float)Ratio);
		vPosition = XMVectorLerp(vSourPosition, vDestPosition, (float)Ratio);
		vPosition = XMVectorSetW(vPosition, 1.f);
	}

	vScale = XMVectorLerp(vBaseScale, vScale, fBlendRatio);
	vRotation = XMQuaternionSlerp(vBaseRot, vRotation, fBlendRatio);
	vPosition = XMVectorLerp(vBasePos, vPosition, fBlendRatio);
	vPosition = XMVectorSetW(vPosition, 1.f);
	
	_matrix TransformMatrix = XMMatrixAffineTransformation(vScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vPosition);

	m_pBone->Set_TransformMatrix(TransformMatrix);
}

void CChannel::Additive_TransformMatrix(_double PlayTime, _float fAdditiveRatio)
{
	_vector vBaseScale, vBaseRot, vBasePos;
	XMMatrixDecompose(&vBaseScale, &vBaseRot, &vBasePos, XMLoadFloat4x4(&m_pBone->Get_TransformMatrix()));

	_vector			vRotation;

	
	if (PlayTime >= m_KeyFrames.back().Time)
	{
		vRotation = XMLoadFloat4(&m_KeyFrames.back().vRotation);
	}
	else
	{
		while (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
		{
			++m_iCurrentKeyFrameIndex;
		}

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);

		_vector			vSourRotation, vDestRotation;
		vSourRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation);
		vDestRotation = XMLoadFloat4(&m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation);
		vRotation = XMQuaternionSlerp(vSourRotation, vDestRotation, (float)Ratio);
	}

	vRotation = XMQuaternionSlerp(XMQuaternionIdentity(), vRotation, fAdditiveRatio);
	vRotation = XMQuaternionMultiply(vBaseRot, vRotation);

	_matrix TransformMatrix = XMMatrixAffineTransformation(vBaseScale, XMVectorSet(0.f, 0.f, 0.f, 1.f), vRotation, vBasePos);

	m_pBone->Set_TransformMatrix(TransformMatrix);
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
	Safe_Release(m_pTrailBone);
	
	m_KeyFrames.clear();
}