#pragma once
#include "Base.h"

#include <io.h>
#include "Fmod/fmod.h"
#include "Fmod/fmod.hpp"
// #pragma comment(lib, "fmodL_vc.lib")

BEGIN(Engine)

class CSound : public CBase
{
public:			
	enum VOLUME_TYPE { VOLUME_DEFAULT, VOLUME_3D_RANGE, VOLUME_USE, VOLUME_END };

	typedef struct ENGINE_DLL tag3DSoundDesc
	{
		// 3D ����� ��� �� true�� �����ؾ� �Ѵ�
		_bool bIs3D; 
		// ������ ���� ��ġ ���� Transform ������Ʈ
		class CTransform* pStartTransform;
		// ������ TargetTransform�� ��ġ�� �������� ���� �ȴ�
		class CTransform* pTargetTransform;
		// ���尡 �鸮�� �Ÿ� pTargetTransform�� ��ġ�� pStartTransform�� ��ġ�� Range ������ �־����� ������ 0
		_float fRange;
	} SOUND_DESC;
	
private:
	FMOD_SOUND* m_pSound = nullptr;	
	SOUND_DESC m_SoundDesc;

	static _float m_fMasterVolume;
	_float m_fVolume[VOLUME_END] = { 0.5f, 1.f, 0.5f };
		
protected:
	CSound();
	virtual ~CSound() = default;

public:	
	virtual void Free() override;
	static CSound* Create(FMOD_SOUND* pSound);
	HRESULT Initialize(FMOD_SOUND* pSound);
	void Tick(double TimeDelta, FMOD_CHANNEL* pChannel);
	FMOD_SOUND* GetSoundPtr() { return m_pSound; }

	void Set_SoundDesc(SOUND_DESC& SoundDesc);
	_float GetVolume(VOLUME_TYPE eType) { return m_fVolume[eType]; }
	void Set_Volume(FMOD_CHANNEL* pChannel, _float fVolume);
	static void Set_MasterVolume(_float fVolume) { m_fMasterVolume = fVolume; }
	void UpdateUseVolume(FMOD_CHANNEL* pChannel);
	void Clear_SoundDesc();
};
END
