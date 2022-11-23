#include "..\Public\Texture.h"
#include "Shader.h"

CTexture::CTexture(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CComponent(pDevice,pContext)
{
}

CTexture::CTexture(const CTexture & rhs)
	:CComponent(rhs)
	, m_pTextures(rhs.m_pTextures)
	, m_iNumTextures(rhs.m_iNumTextures)
{
	for (_uint i = 0; i < m_iNumTextures; ++i)
		Safe_AddRef(m_pTextures[i]);
}

HRESULT CTexture::Init_Prototype(const _tchar * pTextureFilePath, _uint iNumTextures)
{
	m_pTextures = new ID3D11ShaderResourceView*[iNumTextures];

	m_iNumTextures = iNumTextures;

	for (_uint i = 0; i < m_iNumTextures; ++i)
	{
		_tchar	szTexturePath[MAX_PATH] = TEXT("");

		wsprintf(szTexturePath, pTextureFilePath, i);

		/* ���ڿ� �����ϱ�. */
		_tchar			szExt[MAX_PATH] = TEXT("");

		_wsplitpath_s(szTexturePath, nullptr, 0, nullptr, 0, nullptr, 0, szExt, MAX_PATH);

		HRESULT		hr = 0;

		if (!lstrcmp(szExt, TEXT(".tga")))
			return E_FAIL;
		else if (!lstrcmp(szExt, TEXT(".dds")))
			hr = DirectX::CreateDDSTextureFromFile(m_pDevice, szTexturePath, nullptr, &m_pTextures[i]);
		else
			hr = DirectX::CreateWICTextureFromFile(m_pDevice, szTexturePath, nullptr, &m_pTextures[i]);

		if (FAILED(hr))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CTexture::Init(void * pArg)
{
	return S_OK;
}

void CTexture::Imgui_RenderProperty()
{
	ImGui::Text("Texture All Count :");
	ImGui::SameLine();
	ImGui::Text(to_string(m_iNumTextures).c_str());
}

HRESULT CTexture::Bind_ShaderResource(CShader * pShaderCom, const char * pConstantName, _uint iTextureIndex)
{
	if (nullptr == pShaderCom || iTextureIndex >= m_iNumTextures)
		return E_FAIL;

	return pShaderCom->Set_ShaderResourceView(pConstantName, m_pTextures[iTextureIndex]);
}

HRESULT CTexture::Bind_ShaderResources(CShader * pShaderCom, const char * pConstantName)
{
	if (nullptr == pShaderCom)
		return E_FAIL;

	return pShaderCom->Set_ShaderResourceViewArray(pConstantName, m_pTextures, m_iNumTextures);
}

CTexture * CTexture::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pTextureFilePath, _uint iNumTextures)
{
	CTexture*		pInstance = new CTexture(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype(pTextureFilePath, iNumTextures)))
	{
		MSG_BOX("Failed to Created : CTexture");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CTexture::Clone(void * pArg)
{
	CTexture*		pInstance = new CTexture(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTexture");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTexture::Free()
{
	__super::Free();

	for (_uint i = 0; i < m_iNumTextures; ++i)
		Safe_Release(m_pTextures[i]);

	if (!m_bClone)
		Safe_Delete_Array(m_pTextures);
}
