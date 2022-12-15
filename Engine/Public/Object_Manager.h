#pragma once

#include "Base.h"

/* ���ӳ��� �ʿ��� ��ü���� �� ����(CLayer)�� ���� ������ �����Ѵ�. */
/* ��ü���� ������ �����ϰ� �ִ� CLayer���� �����ϴ� Ŭ�����̴�. */

BEGIN(Engine)

class CObject_Manager final : public CBase
{
	DECLARE_SINGLETON(CObject_Manager);
private:
	CObject_Manager();
	virtual ~CObject_Manager() = default;

public:
	class CComponent* Get_ComponentPtr(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex);

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Clear(_uint iLevelIndex);

public:
	HRESULT Add_Prototype(const wstring& pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Clone_GameObject(_uint iLevelIndex,const wstring& pLayerTag,const wstring& pPrototypeTag, void* pArg = nullptr);
	CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, void* pArg = nullptr);

	void	Tick(_double TimeDelta);
	void	Late_Tick(_double TimeDelta);
	
	class CGameObject* Get_GameObject(_uint iLevelIndex, const wstring& pLayerTag, const wstring& strObjName);
	class CLayer* Find_Layer(_uint iLevelIndex, const wstring& pLayerTag);

public: /* imgui */
	// ��� ���� Object�� Imgui�� ����Ѵ�.
	// ������ ������ �̸��� szSelectedProto�� �����ش�.
	void Imgui_ProtoViewer(_uint iLevel, OUT const _tchar*& szSelectedProto);

	// iLevel�� �ִ� ��� �纻 Object�� Layer���� Imgui�� ����Ѵ�.
	// ������ Object�� pSelectedObject�� �����ش�.
	// �ش� Object�� ���ų� ������� nullptr�� �����ش�.
	void Imgui_ObjectViewer(_uint iLevel, OUT CGameObject*& pSelectedObject);

	void SaveData(_uint iLevel, wstring strDirectory);
	void LoadData(_uint iLevel, wstring strDirectory);

private: /* ������ü���� ��Ƴ��´�. */
	map<wstring, class CGameObject*>			m_Prototypes;
	typedef map<wstring, class CGameObject*>	PROTOTYPES;

private: /* �纻��ü���� �����ϱ����� �����̳�. */
	map<wstring, class CLayer*>*				m_pLayers = nullptr;
	typedef map<wstring, class CLayer*>		    LAYERS;

	_uint										m_iNumLevels = 0;
	char										m_szLayerName[256] = {};

private:
	class CGameObject* Find_Prototype(const wstring& pPrototypeTag);

public:
	virtual void Free() override;
};

END