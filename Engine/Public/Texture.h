#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CTexture final : public CComponent
{
protected:
	CTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CTexture(const CTexture& rhs);
	virtual ~CTexture() = default;

public:
	virtual HRESULT Init_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures);
	virtual HRESULT Init(void* pArg) override;

	virtual void Imgui_RenderProperty() override;

	_uint  Get_CntTex() { return m_iNumTextures; }
	ID3D11ShaderResourceView* Get_Texture(_uint index) { return m_pTextures[index]; }

public:
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName, _uint iTextureIndex = 0);
	HRESULT Bind_ShaderResources(class CShader* pShaderCom, const char* pConstantName);

private:
	_uint							m_iNumTextures = 0;
	ID3D11ShaderResourceView**		m_pTextures = nullptr;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END