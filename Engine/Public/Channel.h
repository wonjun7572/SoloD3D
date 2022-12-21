#pragma once

#include "Base.h"

BEGIN(Engine)

class CChannel final : public CBase
{
public:
	typedef struct ChannelLoadTag
	{
		char				mNodeName[MAX_PATH];
		_uint				mNumKeyFrames;
		vector<KEYFRAME>	mKeyFrames;
	}CHANNELLOAD;

private:
	CChannel();
	virtual ~CChannel() = default;

public:
	HRESULT Initialize(CHANNELLOAD& pAIChannel, class CModel* pModel);
	void	Update_TransformMatrix(_double PlayTime);
	void	Reset_KeyFrameIndex() { m_iCurrentKeyFrameIndex = 0; }
	char*	Get_ChannelName(void) { return m_szName; }

	void	Blend_TransformMatrix(_double PlayTime, _float fBlendRatio);
	void	Additive_TransformMatrix(_double PlayTime, _float fAdditiveRatio);

	_float4			Get_MovePos() { return m_vMovePos; }

private:
	char			m_szName[MAX_PATH] = "";
	class CBone*	m_pBone = nullptr;

	_uint				m_iNumKeyframes = 0;
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iCurrentKeyFrameIndex = 0;
	_float4				m_vMovePos = _float4(0.f, 0.f, 0.f, 1.f);

public:
	static CChannel* Create(CHANNELLOAD& pAIChannel, class CModel* pModel);
	virtual void Free() override;
};

END