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
	void	Reset_LerpIndex(void) { m_iLerpFrameIndex = -1; }
	char*	Get_ChannelName(void) { return m_szName; }
	_bool	Lerp_TransformMatrix(_double PlayTime, CChannel* Sour, CChannel* Dest, _bool bFinish = false);

private:
	char			m_szName[MAX_PATH] = "";
	class CBone*	m_pBone = nullptr;

	_uint				m_iNumKeyframes = 0;
	vector<KEYFRAME>	m_KeyFrames;

	_uint				m_iCurrentKeyFrameIndex = 0;
	_int				m_iLerpFrameIndex = -1;
	_double				m_LerpRatio = 0;
public:
	static CChannel* Create(CHANNELLOAD& pAIChannel, class CModel* pModel);
	virtual void Free() override;
};

END