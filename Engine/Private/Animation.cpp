#include "..\Public\Animation.h"
#include "Channel.h"

CAnimation::CAnimation()
{
}

HRESULT CAnimation::Initialize(ANIMATIONLOAD& pAIAnimation, CModel* pModel)
{
	m_ANIMATIONLOAD = pAIAnimation;

	strcpy_s(m_szName, pAIAnimation.mName);

	m_Duration = pAIAnimation.mDuration;

	m_TickPerSecond = pAIAnimation.mTicksPerSecond;

	/* 이 애니메이션 구동하는데 필요한 뼈대의 갯수다.  */
	m_iNumChannels = pAIAnimation.mNumChannels;

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		CChannel*		pChannel = CChannel::Create(pAIAnimation.pvecChannels[i], pModel);
		if (nullptr == pChannel)
			return E_FAIL;

		m_Channels.push_back(pChannel);
	}

	return S_OK;
}

_bool CAnimation::Update_Bones(_double TimeDelta)
{
	if (true == m_isFinished &&
		false == m_isLooping)
	{
		return false;
	}

	m_PlayTime += m_TickPerSecond * TimeDelta;

	if (m_PlayTime >= m_Duration)
	{
		m_PlayTime = 0.0;
		m_isFinished = true;
		return true;
	}

	if (m_isLooping && m_isFinished)
	{
		for (auto pChannel : m_Channels)
			pChannel->Reset_KeyFrameIndex();
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		m_Channels[i]->Update_TransformMatrix(m_PlayTime);
	}

	return false;
}

_bool CAnimation::AnimLerpTime(_double TimeDelta, CAnimation * pNext, _bool bFinish)
{
	if (m_isLerping)
	{
		m_PlayTime = 0.f;

		for (auto pChannel : m_Channels)
		{
			pChannel->Reset_KeyFrameIndex();
			pChannel->Reset_LerpIndex();
		}

		m_isLerping = false;

		return false;
	}
	else
	{
		for (auto pChannel : m_Channels)
		{
			for (auto pNext : pNext->m_Channels)
			{
				if (!strcmp(pChannel->Get_ChannelName(), pNext->Get_ChannelName()))
				{
					m_isLerping = pChannel->Lerp_TransformMatrix(TimeDelta, pChannel, pNext, bFinish);
					break;
				}
			}
		}

		return true;
	}

	return true;
}

CAnimation * CAnimation::Create(ANIMATIONLOAD& pAIAnimation, CModel* pModel)
{
	CAnimation* pInstance = new CAnimation;

	if (FAILED(pInstance->Initialize(pAIAnimation, pModel)))
	{
		MSG_BOX("Failed to Create : CAnimation");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CAnimation::Free()
{
	for (auto& pChannel : m_Channels)
		Safe_Release(pChannel);

	m_Channels.clear();
}
