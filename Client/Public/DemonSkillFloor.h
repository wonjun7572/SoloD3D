#pragma once

#include "Effect_Rect.h"

class CDemonSkillFloor final : public CEffect_Rect
{
private:
	CDemonSkillFloor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDemonSkillFloor(const CEffect_Rect& rhs);
	virtual ~CDemonSkillFloor() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	CTexture* m_pMaskTexCom = nullptr;

public:
	static CDemonSkillFloor* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

