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

void CAnimation::Reset()
{
	m_PlayTime = 0.0; 
	m_isFinished = false; 
	for (auto& pChannel : m_Channels)
		pChannel->Reset_KeyFrameIndex();
}

void CAnimation::Update_Bones(_double TimeDelta)
{
	if (true == m_isFinished &&
		false == m_isLooping)
	{
		return;
	}

	m_PlayTime += m_TickPerSecond * TimeDelta * m_MulSecond;

	if (m_PlayTime >= m_Duration)
	{
		m_PlayTime = 0.0;
		m_isFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Reset_KeyFrameIndex();

		m_Channels[i]->Update_TransformMatrix(m_PlayTime);
	}

	if (m_isFinished == true)
		m_isFinished = false;
}

void CAnimation::Update_Bones_Blend(_double TimeDelta, _float fBlendRatio)
{
	if (true == m_isFinished &&
		false == m_isLooping)
	{
		return;
	}

	m_PlayTime += m_TickPerSecond * TimeDelta * m_MulSecond;

	if (m_PlayTime >= m_Duration)
	{
		m_PlayTime = 0.0;
		m_isFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Reset_KeyFrameIndex();

		m_Channels[i]->Blend_TransformMatrix(m_PlayTime, fBlendRatio);
	}

	if (m_isFinished == true)
		m_isFinished = false;
}

void CAnimation::Update_Bones_Add(_double TimeDelta, _float fAdditiveRatio)
{
	if (true == m_isFinished &&
		false == m_isLooping)
	{
		return;
	}

	m_PlayTime += m_TickPerSecond * TimeDelta;

	if (m_PlayTime >= m_Duration)
	{
		m_PlayTime = 0.0;
		m_isFinished = true;
	}

	for (_uint i = 0; i < m_iNumChannels; ++i)
	{
		if (true == m_isFinished)
			m_Channels[i]->Reset_KeyFrameIndex();

		if (!strcmp("Glove.ao", m_Channels[i]->Get_ChannelName()))
			continue;
		
		if (!strcmp("Upper", m_Channels[i]->Get_ChannelName()))
			continue;

		if (!strcmp("Lower.ao", m_Channels[i]->Get_ChannelName()))
			continue;

		if (!strcmp("Face.ao", m_Channels[i]->Get_ChannelName()))
			continue;

		if (!strcmp("AN_PC_HumA_F_MS0001_A00_Shoulder.ao", m_Channels[i]->Get_ChannelName()))
			continue;

		if (!strcmp("Belt.ao", m_Channels[i]->Get_ChannelName()))
			continue;

		if (!strcmp("Boots.ao", m_Channels[i]->Get_ChannelName()))
			continue;

		if (!strcmp("Helmet.ao", m_Channels[i]->Get_ChannelName()))
			continue;

		if(!strcmp("SkeletonWarrior_SK_ATR.ao", m_Channels[i]->Get_ChannelName()))
			continue;

		if (!strcmp("Skeletonwolf_SK_KVN.ao", m_Channels[i]->Get_ChannelName()))
			continue;

		m_Channels[i]->Additive_TransformMatrix(m_PlayTime, fAdditiveRatio);
	}
}

_bool CAnimation::Check_AnimationSet(const _float & fTime)
{
	if (m_Duration <= m_PlayTime + fTime)
		return true;

	return false;
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
