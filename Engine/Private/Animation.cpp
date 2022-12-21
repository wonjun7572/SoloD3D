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

void CAnimation::Update_Bones(_double TimeDelta)
{
	m_vMovePos = _float4(0.f, 0.f, 0.f, 1.f);

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

		m_Channels[i]->Update_TransformMatrix(m_PlayTime);

		if (!strcmp("BN_Head", m_Channels[i]->Get_ChannelName()))
			m_vMovePos = m_Channels[i]->Get_MovePos();
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

		m_Channels[i]->Additive_TransformMatrix(m_PlayTime, fAdditiveRatio);
	}
}

void CAnimation::Set_IsFinish(_bool isFinish)
{
	Reset_PlayTime();
	m_isFinished = isFinish;
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
