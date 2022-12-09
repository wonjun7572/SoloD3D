
#pragma once
#include "Client_Define.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
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

	HRESULT	Ready_FilterBuffer();
	HRESULT DynanicBuffer_ForBrush();

private:
	CShader*					m_pShaderCom = nullptr;
	CRenderer*					m_pRendererCom = nullptr;
	CTexture*					m_pTextureCom[TYPE_END] = { nullptr };
	CVIBuffer_Terrain*			m_pVIBufferCom = nullptr;

	ID3D11Texture2D*			m_pTexture2D = nullptr;
	D3D11_TEXTURE2D_DESC		m_TextureDesc;

private: /* For. Imgui*/

	_float			m_fHeight = 0.f;

	bool			m_bPicking = false;
	bool			m_bDefaultHeight = false;

	int				m_iPassNum = 0;

	int				m_iTexRadioBtn = 0;

	_uint			m_iDiffuseTexCnt = 0;
	_uint			m_iDiffuseATexNum = 3;
	_uint			m_iDiffuseBTexNum = 4;

	_uint			m_iBrushTexNum = 0;
	_float4			m_vBrushPos = _float4(0.f, 0.f, 0.f, 1.f);
	_float			m_fBrushRange = 5.f;

	_uint			m_iFilterTexNum = 0;

	// TODO : Brush
	_ulong*			m_pPixel = nullptr;

	_bool			m_bHeight = false;
	_bool			m_bFilter = false;

private:
	HRESULT SetUp_Components();
	HRESULT SetUp_ShaderResources();

public:
	static CTerrain* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END