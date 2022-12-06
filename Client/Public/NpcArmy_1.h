#pragma once

#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CRenderer;
class CModel;
END

BEGIN(Client)

class CNpcArmy_1 final : public CGameObject
{
private:
	CNpcArmy_1(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CNpcArmy_1(const CNpcArmy_1& rhs);
	virtual ~CNpcArmy_1() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty() override;

private:
	CShader*				m_pShaderCom = nullptr;
	CRenderer*				m_pRendererCom = nullptr;
	CModel*					m_pModelCom = nullptr;
private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

	_uint m_iCurrentAnimIndex = 33;

public:
	static CNpcArmy_1* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END

