#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CModel;
class CShader;
class CRenderer;
class CNavigation;
class CAnimation;
class CFSMComponent;
END

BEGIN(Client)

class CHorse final : public CGameObject
{
public:
	enum HORSE_DIRECTION
	{
		HORSE_FM, HORSE_BM, PLAYER_END
	};

	enum ANIMATION
	{
		HORSE_ADD_BRAKE, 
		HORSE_ADD_ROT_L,
		HORSE_ADD_ROT_POSE_L,
		HORSE_ADD_ROT_POSE_R,
		HORSE_ADD_ROT_R,
		HORSE_BRAKE,
		HORSE_IDLE,
		HORSE_ROT_L_LOOP,
		HORSE_ROT_L_START,
		HORSE_ROT_R_LOOP,
		HORSE_ROT_R_START,
		HORSE_JUMP_LAND,
		HORSE_ROT_L,
		HORSE_ROT_POSE_L,
		HORSE_ROT_POSE_R,
		HORSE_ROT_R,
		HORSE_RUN_B,
		HORSE_RUN_F,
		HORSE_RUN_F_STOP,
		HORSE_RUN_JUMP_LAND,
		HORSE_RUN_JUMP_LOOP,
		HORSE_RUN_JUMP_UP,
		HORSE_SPECIAL,
		HORSE_STATE_END
	};

public:
	CHorse(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHorse(const CHorse& rhs);
	virtual ~CHorse() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;
	virtual HRESULT RenderShadow() override;

	void    SetUp_FSM();
	void	  AdditiveAnim(_double TimeDelta);
	void	  LinkPrince(_double TimeDelta);

	void	Reset_Anim(ANIMATION eAnim);
	void	Set_Anim(ANIMATION eAnim);

private:
	void	Imgui_RenderProperty() override;

	CModel*					Get_ModelCom() { return m_pModelCom; }

private:
	CModel*					m_pModelCom =  nullptr ;
	CShader*					m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CNavigation*			m_pNavigationCom = nullptr;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();
	HRESULT	SetUP_ShadowShaderResources();

	class CPrincePlayer* m_pPrincePlayer = nullptr;

public:
	static CHorse* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END