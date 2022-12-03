#include "..\Public\Channel.h"
#include "Model.h"
#include "Bone.h"
#include "MathUtils.h"

CChannel::CChannel()
{
}

HRESULT CChannel::Initialize(aiNodeAnim * pAIChannel, CModel * pModel)
{
	strcpy_s(m_szName, pAIChannel->mNodeName.data);

	m_pBone = pModel->Get_BonePtr(m_szName);
	Safe_AddRef(m_pBone);

	m_iNumKeyframes = max(pAIChannel->mNumPositionKeys,max(pAIChannel->mNumScalingKeys, pAIChannel->mNumRotationKeys));

	XMFLOAT3 vScale, vPosition;
	XMFLOAT4 vRotation;

	for (_uint i = 0; i < m_iNumKeyframes; ++i)
	{
		KEYFRAME keyFrame;
		ZeroMemory(&keyFrame, sizeof(KEYFRAME));

		if (i < pAIChannel->mNumScalingKeys)
		{
			memcpy(&vScale, &pAIChannel->mScalingKeys[i].mValue, sizeof(XMFLOAT3));
			keyFrame.Time = pAIChannel->mScalingKeys[i].mTime;
		}

		if (i < pAIChannel->mNumRotationKeys)
		{
			vRotation.x = pAIChannel->mRotationKeys[i].mValue.x;
			vRotation.y = pAIChannel->mRotationKeys[i].mValue.y;
			vRotation.z = pAIChannel->mRotationKeys[i].mValue.z;
			vRotation.w = pAIChannel->mRotationKeys[i].mValue.w;
			keyFrame.Time = pAIChannel->mRotationKeys[i].mTime;
		}

		if (i < pAIChannel->mNumPositionKeys)
		{
			memcpy(&vPosition, &pAIChannel->mPositionKeys[i].mValue, sizeof(XMFLOAT3));
			keyFrame.Time = pAIChannel->mPositionKeys[i].mTime;
		}

		keyFrame.vScale = vScale;
		keyFrame.vRotation = vRotation;
		keyFrame.vPosition = vPosition;

		m_KeyFrames.push_back(keyFrame);
	}

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
		if (PlayTime >= m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time)
				++m_iCurrentKeyFrameIndex;

		_double			Ratio = (PlayTime - m_KeyFrames[m_iCurrentKeyFrameIndex].Time) /
			(m_KeyFrames[m_iCurrentKeyFrameIndex + 1].Time - m_KeyFrames[m_iCurrentKeyFrameIndex].Time);
	
		vScale = CMathUtils::Lerp(m_KeyFrames[m_iCurrentKeyFrameIndex].vScale, m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vScale, static_cast<float>(Ratio));
		vRotation = CMathUtils::QuaternionSlerp(m_KeyFrames[m_iCurrentKeyFrameIndex].vRotation, m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vRotation, static_cast<float>(Ratio));
		vPosition = CMathUtils::Lerp(m_KeyFrames[m_iCurrentKeyFrameIndex].vPosition, m_KeyFrames[m_iCurrentKeyFrameIndex + 1].vPosition, static_cast<float>(Ratio));
	}

	TransformMatrix = CMathUtils::MatrixAffineTransformation(_float4(vScale.x,vScale.y,vScale.z, 0.f), _float4(0.f, 0.f, 0.f, 1.f), vRotation,_float4(vPosition.x,vPosition.y,vPosition.z,1.f));

	m_pBone->Set_TransformMatrix(TransformMatrix);
}

CChannel * CChannel::Create(aiNodeAnim * pAIChannel, CModel * pModel)
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
}

