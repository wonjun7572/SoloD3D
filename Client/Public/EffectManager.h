#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameObject;
class CTransform;
END

BEGIN(Client)

class CEffectManager final : public CBase
{
	DECLARE_SINGLETON(CEffectManager)

public:
	CEffectManager();
	virtual	~CEffectManager() = default;

	HRESULT Add_Effects(const wstring& pPrototypeEffectTag, const wstring& pEffectTag, void* pArg = nullptr);
	HRESULT Render_Effects(const wstring& pEffectTag, _double TimeDelta);
	HRESULT	LinkObject(const wstring& pEffectTag, _float4 vTargetPos, _float4 vLook = _float4(0.f, 0.f, 0.f, 0.f));
	HRESULT SetUp_Effects(const wstring& pEffectTag, _float fUVSpeed, _float fAlpha, _float2 UV = _float2(0.f, 0.f));

	class CTransform*  Get_Transform(const wstring& pEffectTag);
	class CGameObject* Find_Effects(const wstring& pEffectTag);
	
private:
	map<wstring, class CGameObject*>			m_Effects;
	typedef map<wstring, class CGameObject*>	EFFECTS;

public:
	virtual void Free() override;
};

END