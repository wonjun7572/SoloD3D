#pragma once

#include "Effect_Rect.h"

class CDemonSkillCircular final : public CEffect_Rect
{
private:
	CDemonSkillCircular(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CDemonSkillCircular(const CEffect_Rect& rhs);
	virtual ~CDemonSkillCircular() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty() override;

	void	Set_Amount(_float fAmount);
	_float  Get_Amount() { return m_fAmount; }
	
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	CTexture* m_pMaskTexCom = nullptr;
	_float	m_fAmount = 0.f;

public:
	static CDemonSkillCircular* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr);
	virtual void Free() override;
};

