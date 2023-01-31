#pragma once

#include "Effect_Rect.h"

class CDemonSkillStraight final : public CEffect_Rect
{
private:
	CDemonSkillStraight(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDemonSkillStraight(const CEffect_Rect& rhs);
	virtual ~CDemonSkillStraight() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	CTexture*	m_pMaskTexCom = nullptr;
	_float		m_fAmount = 0.5f;

public:
	static CDemonSkillStraight* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

