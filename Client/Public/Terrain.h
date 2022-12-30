
#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CNavigation;
class CVIBuffer_Terrain;
END

BEGIN(Client)

class CTerrain final : public CGameObject
{
public:
	enum TEXTURE { TYPE_DIFFUSE, TYPE_BRUSH, TYPE_FILTER, TYPE_END };

private:
	CTerrain(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTerrain(const CTerrain& rhs);
	virtual ~CTerrain() = default;

public:
	virtual HRESULT Init_Prototype() override;
	virtual HRESULT Init(void* pArg) override;
	virtual void Tick(_double TimeDelta) override;
	virtual void Late_Tick(_double TimeDelta) override;
	virtual HRESULT Render() override;

	virtual void Imgui_RenderProperty() override;

private:/* For. Imgui*/
	_float4	PickingOnTerrain(const CVIBuffer_Terrain* pTerrainBufferCom, const CTransform* pTerrainTransformCom);
	_bool	PickingForFilter(const CVIBuffer_Terrain* pTerrainBufferCom, const CTransform* pTerrainTransformCom);
	void	Add_NaviCell(HWND hWnd, _uint iWinsizeX, _uint iWinsizey, const class CVIBuffer_Terrain* pTerrainBufferCom, const class CTransform* pTerrainTransformCom);

	HRESULT	Ready_FilterBuffer();
	HRESULT Dynamic_FilterBuffer();

	HRESULT	Dynamic_Navi();
	HRESULT	Save_Navi(_int iIndex);

	HRESULT Load_CubeList(_int iIndex);
	HRESULT Save_CubeList(_int iIndex);


	void AdjustCellPoint();

private:
	_bool	m_bDeleteCell = false;
	_uint	m_iDeleteCellNum = 0;

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom[TYPE_END] = { nullptr };
	CVIBuffer_Terrain*			m_pVIBufferCom = nullptr;
	CNavigation*				m_pNavigationCom = nullptr;

private: /* For. Imgui*/

	int				m_iChapNum = 2;

	_float			m_fHeight = 0.f;

	bool			m_bPicking = false;
	bool			m_bDefaultHeight = false;

	int				m_iPassNum = 0;
	
	_bool			m_bFilter = false;
	_bool			m_bHeight = false;

	int				m_iTexRadioBtn = 0;

	_uint			m_iDiffuseTexCnt = 0;
	_uint			m_iDiffuseATexNum = 3;
	_uint			m_iDiffuseBTexNum = 4;

	_uint			m_iBrushTexNum = 0;
	_float4			m_vBrushPos = _float4(0.f, 0.f, 0.f, 1.f);
	_float			m_fBrushRange = 5.f;

	_uint			m_iFilterTexNum = 0;

	_ulong*			m_pPixel = nullptr;

	_bool			m_bNavi = false;

	_bool			m_bRealNaviSetUp = false;

	_uint				m_iNaviPointCount = 0;

	_float3				m_vPoints[3];
	list<CGameObject*>	m_pCubeList;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END