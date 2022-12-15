#pragma once

#include "Base.h"

/* 게임내에 필요한 객체들을 내 기준(CLayer)에 따라 나누어 보관한다. */
/* 객체들을 나누어 저장하고 있는 CLayer들을 보관하는 클래스이다. */

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
	// 모든 원본 Object를 Imgui로 출력한다.
	// 선택한 원본의 이름을 szSelectedProto로 돌려준다.
	void Imgui_ProtoViewer(_uint iLevel, OUT const _tchar*& szSelectedProto);

	// iLevel에 있는 모든 사본 Object를 Layer별로 Imgui로 출력한다.
	// 선택한 Object는 pSelectedObject로 돌려준다.
	// 해당 Object가 없거나 사라지면 nullptr로 돌려준다.
	void Imgui_ObjectViewer(_uint iLevel, OUT CGameObject*& pSelectedObject);

	void SaveData(_uint iLevel, wstring strDirectory);
	void LoadData(_uint iLevel, wstring strDirectory);

private: /* 원형객체들을 모아놓는다. */
	map<wstring, class CGameObject*>			m_Prototypes;
	typedef map<wstring, class CGameObject*>	PROTOTYPES;

private: /* 사본객체들을 보관하기위한 컨테이너. */
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