#pragma once

#include "Base.h"
#include "Channel.h"

BEGIN(Engine)

class CAnimation final : public CBase
{
public:
	typedef struct AnimationLoadTag
	{
		char		mName[MAX_PATH];
		_double		mDuration;
		_double		mTicksPerSecond;
		_uint		mNumChannels;
		_bool		mIsLooping;
		_bool		mIsFinished;
		vector<CChannel::CHANNELLOAD> pvecChannels;
	}ANIMATIONLOAD;

public:
	CAnimation();
	virtual ~CAnimation() = default;

public:
	HRESULT Initialize(ANIMATIONLOAD& pAIAnimation, class CModel* pModel);
	_bool	Update_Bones(_double TimeDelta);
	_bool	AnimLerpTime(_double TimeDelta, CAnimation * pNext, _bool bFinish);

public:
	char*	Get_AnimationName() { return m_szName; }
	ANIMATIONLOAD&	Get_ANIMATIONLOAD() { return m_ANIMATIONLOAD; }

	void	Set_isLooping(_bool isLooping) { m_isLooping = isLooping; }

public:
	void		Reset_PlayTime(void) { m_PlayTime = 0.0; }
	_vector		Get_MovePos(void) { return XMLoadFloat3(&m_vMovePos); }
	_double		Get_Duration() { return m_Duration; }

private:
	char								m_szName[MAX_PATH];
	_double								m_Duration = 0.0;
	_double								m_TickPerSecond;

	_double								m_PlayTime = 0.0;

	_bool								m_isLooping = true;
	_bool								m_isFinished = false;
	_bool								m_isLerping = false;

	/* 이 애니메이션을 재생하기위해 갱신해야하는 뼈들. */
	_uint								m_iNumChannels = 0;
	vector<class CChannel*>				m_Channels;
	ANIMATIONLOAD						m_ANIMATIONLOAD;

private:
	_float3								m_vMovePos;

public:
	static CAnimation* Create(ANIMATIONLOAD& pAIAnimation, class CModel* pModel);
	virtual void Free() override;
};

END