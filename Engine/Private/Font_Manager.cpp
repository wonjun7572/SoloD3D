#include "..\Public\Font_Manager.h"
#include "CustomFont.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CFont_Manager)

CFont_Manager::CFont_Manager()
{
}

HRESULT CFont_Manager::Add_Fonts(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const wstring & pFontTag, const wstring & pFontFilePath)
{
	if (nullptr != Find_Fonts(pFontTag))
		return E_FAIL;

	CCustomFont*		pFont = CCustomFont::Create(pDevice, pContext, pFontFilePath);

	if (nullptr == pFont)
		return E_FAIL;

	m_Fonts.insert({ pFontTag, pFont });

	return S_OK;
};

HRESULT CFont_Manager::Render_Font(const _tchar* pFontTag, const _tchar* pText, const _float2& vPos, _float fRadian, _float2 vScale, _fvector vColor)
{
	CCustomFont*		pFont = Find_Fonts(pFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	return pFont->Render(pText, vPos, fRadian, vScale, vColor);
}

HRESULT CFont_Manager::Render_Font(const _tchar * pFontTag, const _tchar * pText, _fvector vPosition, _fvector vColor, _float fRadian, _fvector vScale)
{
	CCustomFont*		pFont = Find_Fonts(pFontTag);

	if (nullptr == pFont)
		return E_FAIL;

	return pFont->DrawTextInWorld(pText, vPosition, vColor, fRadian, vScale);
}

void CFont_Manager::DrawTextInWorld(const _tchar* pFontTag, const _tchar * text, _float4 vPos, _float3 vScale, _float4 vColor)
{
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float4 vCamPos = pGameInstance->Get_CamPosition();

	_float fDistance = _float4::Distance(vCamPos, vPos);

	if (fDistance >= 250.f)
	{
		RELEASE_INSTANCE(CGameInstance);
		return;
	}

	_float3 vResultPos;
	_matrix	ResultMatrix;
	_matrix	WolrdMatrix;
	_matrix PosMatrix;
	_matrix ScaleMatrix;
	_matrix ViewMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);
	_matrix ProjMatrix = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	PosMatrix = XMMatrixTranslation(vPos.x, vPos.y, vPos.z);
	ScaleMatrix = XMMatrixScaling(vScale.x, vScale.y, 1.f);
	WolrdMatrix = ScaleMatrix * PosMatrix;

	D3D11_VIEWPORT			ViewportDesc;
	ZeroMemory(&ViewportDesc, sizeof ViewportDesc);

	_uint			iNumViewports = 1;
	pGameInstance->Get_Context()->RSGetViewports(&iNumViewports, &ViewportDesc);

	vResultPos = XMVector3Project(vResultPos, ViewportDesc.TopLeftX, ViewportDesc.TopLeftY, ViewportDesc.Width, ViewportDesc.Height, ViewportDesc.MinDepth, ViewportDesc.MaxDepth, ProjMatrix, ViewMatrix, WolrdMatrix);

	RELEASE_INSTANCE(CGameInstance);

	Render_Font(pFontTag,text, _float4(vResultPos.x,vResultPos.y, vResultPos.z,1.f), vColor, 0.f, vScale);
}

CCustomFont * CFont_Manager::Find_Fonts(const wstring & pFontTag)
{
	auto iter = m_Fonts.find(pFontTag);

	if (iter == m_Fonts.end())
		return nullptr;

	return iter->second;
}

void CFont_Manager::Free()
{
	for (auto& Pair : m_Fonts)
		Safe_Release(Pair.second);

	m_Fonts.clear();
}
