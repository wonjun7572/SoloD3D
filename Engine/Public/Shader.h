#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CShader final : public CComponent
{
protected:
	CShader(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CShader(const CShader& rhs);
	virtual ~CShader() = default;

public:
	virtual HRESULT Init_Prototype(const wstring& pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements);
	virtual HRESULT Init(void* pArg) override;

	virtual void Imgui_RenderProperty() override;
	_uint Get_NumPasses() { return m_iNumPasses; }

public:
	HRESULT Begin(_uint iPassIndex);
	HRESULT Set_RawValue(const char* pConstantName, const void* pData, _uint iLength);
	HRESULT	Set_Matrix(const char* pConstantName, const _float4x4* pMatrix);

	HRESULT Set_ShaderResourceView(const char* pConstantName, ID3D11ShaderResourceView* pSRV);
	HRESULT Set_ShaderResourceViewArray(const char* pConstantName, ID3D11ShaderResourceView** ppSRV, _uint iNumTextures);
	
	HRESULT Set_MatrixArray(const char* pConstantName, const _float4x4* pData, _uint iNumMatrices);
	HRESULT ReCompile();

private:
	ID3DX11Effect*		m_pEffect = nullptr;
	vector<ID3D11InputLayout*>	m_InputLayouts;

	wstring							m_wstrFilePath = L"";
	const D3D11_INPUT_ELEMENT_DESC* m_pElements = nullptr;
	_uint							m_iNumElements = 0;

private:
	_uint				m_iNumPasses = 0;

public:
	static CShader* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const wstring& pShaderFilePath, const D3D11_INPUT_ELEMENT_DESC* pElements, const _uint iNumElements);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};


END
