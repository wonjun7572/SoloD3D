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

	const vector<ID3D11ShaderResourceView*>& Get_Texture() { return m_Textures; }
	_uint  Get_CurNumTex() { return m_iNumTextures; }
	size_t  Get_AllTexsize() { return m_Textures.size(); }

public:
	HRESULT Bind_ShaderResource(class CShader* pShaderCom, const char* pConstantName, _uint iTextureIndex = 0);

private:
	_uint						m_iNumTextures = 0;
	vector<ID3D11ShaderResourceView*>			m_Textures;
	typedef vector<ID3D11ShaderResourceView*>	TEXTURES;

public:
	static CTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END