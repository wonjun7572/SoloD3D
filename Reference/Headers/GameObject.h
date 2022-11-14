#pragma once

#include "Base.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*			m_pDevice = nullptr;
	ID3D11DeviceContext*	m_pContext = nullptr;

public: /* imgui */
		// �� ������Ʈ�� ������ �ִ� component�� Imgui_RenderProtpery�Լ��� �����ϴ� �Լ�.
	void Imgui_RenderComponentProperties();

	// �� ������Ʈ���� ������ �����͸� imgui�� �ۼ��Ѵ�.
	virtual void Imgui_RenderProperty() {}

protected:
	/* ��ü���� ����ؾ� �� ������Ʈ���� �����Ѵ�. */
	map<wstring, class CComponent*>			m_Components;

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	class CComponent* Find_Component(const wstring& pComponentTag);

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END