#pragma once

#include "Renderer.h"
#include "Shader.h"
#include "Texture.h"

#include "VIBuffer_Rect.h"
#include "VIBuffer_Sphere.h"
#include "VIBuffer_Cube.h"
#include "VIBuffer_Terrain.h"
#include "VIBuffer_Cylinder.h"
#include "VIBuffer_Rect_Instancing.h"

#include "Transform.h"

#include "FSMComponent.h"

#include "Model.h"
#include "Collider.h"
#include "Navigation.h"

BEGIN(Engine)

class CComponent_Manager final : public CBase
{
	DECLARE_SINGLETON(CComponent_Manager)
private:
	CComponent_Manager();
	virtual ~CComponent_Manager() = default;

public:
	HRESULT Reserve_Manager(_uint iNumLevels);
	HRESULT Add_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const wstring& pPrototypeTag, void* pArg = nullptr);
	void	Remove_ProtoComponent(_uint iLevel, const wstring & pComponentName);

	void Imgui_ComponentViewer(_uint iLevel, OUT const _tchar*& szSelectedProto);
	
private:
	_uint										m_iNumLevels = 0;

	/* 레벨별로 원형 컴포넌트(텍스쳐, 메시모델 etc )들을 보관하겠다. */
	map<wstring, class CComponent*>*			m_pPrototypes = nullptr;
	typedef map<wstring, class CComponent*>		PROTOTYPES;

private:
	CComponent* Find_Prototype(_uint iLevelIndex, const wstring& pPrototypeTag);

public:
	virtual void Free() override;
};

END