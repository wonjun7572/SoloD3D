#pragma once
#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CUI abstract : public CGameObject
{
protected:
	CUI(ID3D11Device*	pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Init_Prototype()override;
	virtual HRESULT Init(void* pArg)override;

	virtual void	Tick(_double TimeDelta)override;
	virtual void	Late_Tick(_double TimeDelta)override;
	virtual HRESULT Render()override;
	virtual void	Set_parentName(_uint iCulLevel, const _tchar* pParentTag);

	
protected:
	class	CShader*   m_pShaderCom = nullptr;
	class	CRenderer* m_pRendererCom = nullptr;
	class	CTexture*  m_pTextureCom = nullptr;
	class	CVIBuffer_Rect*	m_pVIBufferCom = nullptr;

protected:
	_float4x4 m_ViewMatrix;
	_float4x4 m_ProjMatrix;
	_float m_fX, m_fY, m_fSizeX, m_fSizeY;

public:
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END