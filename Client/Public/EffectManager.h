#pragma once

#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Client)

class CEffectManager final : public CBase
{
	DECLARE_SINGLETON(CEffectManager)

	enum EffectOption { OPTION_RECT, OPTION_POINT, OPTION_MESH, OPTION_END};

public:
	CEffectManager();
	virtual	~CEffectManager() = default;

	HRESULT Add_Effects(const wstring& pPrototypeEffectTag, const wstring& pEffectTag, void* pArg = nullptr);
	HRESULT Render_Effects(const wstring& pEffectTag, _double TimeDelta);
	class CGameObject* Find_Effects(const wstring& pEffectTag);
	
private:
	map<wstring, class CGameObject*>			m_Effects;
	typedef map<wstring, class CGameObject*>	EFFECTS;

public:
	virtual void Free() override;
};

END