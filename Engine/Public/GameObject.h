#pragma once

#include "Transform.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
public:
	typedef struct  tagGameObjectDesc
	{
		CTransform::TRANSFORMDESC TransformDesc;
	}GAMEOBJECTDESC;

protected:
	CGameObject(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	static wstring m_pTransformComTag;
	map<wstring, class CComponent*>& GetComponents() { return m_Components; }
	const wstring& Get_ObjName() { return m_strObjName; }
	CTransform*	Get_TransformCom() {return m_pTransformCom;}

public:
	virtual HRESULT Init_Prototype();
	virtual HRESULT Init(void* pArg);
	virtual void Tick(_double TimeDelta);
	virtual void Late_Tick(_double TimeDelta);
	virtual HRESULT Render();
	virtual	HRESULT RenderShadow();

	virtual void Set_ModelTag(_tchar* szTag){wcscpy_s(m_szModelTag,MAX_PATH, szTag);}
	virtual const _tchar*	Get_ModelTag() { return nullptr; }

	virtual void Set_ObjectName(const wstring& strTag) { m_strObjName = strTag; }
	virtual const wstring&  Get_ObjectName() { return m_strObjName; }
	virtual void	Remove_Component(const wstring& strTag);

	bool		Get_Dead() { return m_bDead; }
	bool		Get_HasShadow() { return m_bHasShadow; }

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
	CTransform*								m_pTransformCom = nullptr;
	_tchar*									m_szModelTag = nullptr;
	wstring									m_strObjName;

	_bool									m_bClone;
	_bool									m_bDead;
	_bool									m_bHasShadow = false;
	_float									m_fCamDistance = { 0.f };
	_float4									m_vRimColor = { _float4(0.f,0.f,0.f,0.f) };

protected:
	HRESULT Add_Component(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	void	Compute_CamDistance();

public:                                                             
	class CComponent* Find_Component(const wstring& pComponentTag);
	_float Get_CamDistance() const { return m_fCamDistance; }

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END