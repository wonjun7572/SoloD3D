#pragma once

#include "Base.h"
#include "Channel.h"

BEGIN(Engine)

class ENGINE_DLL CAnimation final : public CBase
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

	void	Update_Bones(_double TimeDelta);
	void	Update_Bones_Blend(_double TimeDelta, _float fBlendRatio);
	void	Update_Bones_Add(_double TimeDelta, _float fAdditiveRatio);
	
public:
	char*	Get_AnimationName() { return m_szName; }
	ANIMATIONLOAD&	Get_ANIMATIONLOAD() { return m_ANIMATIONLOAD; }

public:
	void		Reset() { m_PlayTime = 0.0; m_isFinished = false; }
	void		Reset_PlayTime(void) { m_PlayTime = 0.0; }
	_float4		Get_MovePos(void) { return m_vMovePos; }
	_double		Get_Duration() { return m_Duration; }

	void		Set_IsFinish(_bool isFinish);
	_bool		Get_IsFinish() { return m_isFinished; }

	void		Set_IsLoop(_bool isLoop) { m_isLooping = isLoop; }
	_bool		Get_IsLoop() { return m_isLooping; }

	char*		Get_Name() { return m_szName; }

	_bool		Check_AnimationSet(const _float& fTime);

private:
	char								m_szName[MAX_PATH];
	_double								m_Duration = 0.0;
	_double								m_TickPerSecond;

	_double								m_PlayTime = 0.0;

	_bool								m_isLooping = true;
	_bool								m_isFinished = false;

	/* 이 애니메이션을 재생하기위해 갱신해야하는 뼈들. */
	_uint								m_iNumChannels = 0;
	vector<class CChannel*>				m_Channels;
	ANIMATIONLOAD						m_ANIMATIONLOAD;

private:
	_float4								m_vMovePos = _float4(0.f,0.f,0.f,1.f);

public:
	static CAnimation* Create(ANIMATIONLOAD& pAIAnimation, class CModel* pModel);
	virtual void Free() override;
};

END