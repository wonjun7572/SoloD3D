#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
class CBone;
class CCollider;
END

BEGIN(Client)

class CParts abstract : public CGameObject
{
public:
	enum MODEL { MODEL_NOMAL, MODEL_A, MODEL_END };

protected:
	CParts(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CParts(const CParts& rhs);
	virtual ~CParts() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	void Set_AnimIndex(_uint iIndex) { m_iAnimIndex = iIndex; }
	void Play_AdditiveAnim(_double TimeDelta, float fRatio);
	void Set_AdditiveAnimIndex(_uint iIndex) { m_iAdditiveAnimIndex = iIndex; }

	void LinkPlayer(CTransform* pTarget);

	CModel* Get_ModelCom() { return m_pModelCom[m_eModelState]; }

	HRESULT PartsRender(_uint iPassIndex);
	HRESULT SetUp_ShaderResources();
	HRESULT SetUp_SecondShaderResources(_float4x4 matrix);

	void	ChangeModel(MODEL iModelIndex);

protected:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	
	CModel*					m_pModelCom[MODEL_END] = { nullptr };
	MODEL					m_eModelState = MODEL_NOMAL;
	MODEL					m_ePreModelState = MODEL_END;

	_uint					m_iAnimIndex = 0;
	_uint					m_iAdditiveAnimIndex = 0;

public:
	void	Imgui_RenderProperty();

protected:
	virtual HRESULT SetUp_Components() = 0;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END