#include "..\public\Light.h"
#include "Shader.h"
#include "VIBuffer_Rect.h"
#include "GameInstance.h"

CLight::CLight(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CLight::Init(const LIGHTDESC & LightDesc)
{
	m_LightDesc = LightDesc;

	
	return S_OK;
}

HRESULT CLight::Render(CVIBuffer_Rect * pVIBuffer, CShader * pShader)
{
	_uint		iPassIndex = 1;

	if (LIGHTDESC::TYPE_DIRECTIONAL == m_LightDesc.eType)
	{
		if (FAILED(pShader->Set_RawValue("g_vLightDir", &m_LightDesc.vDirection, sizeof(_float4))))
			return E_FAIL;

		CGameInstance* pInst = GET_INSTANCE(CGameInstance);

		if (FAILED(pShader->Set_Matrix("g_LightViewMatrix", &pInst->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTVIEW))))
			return E_FAIL;
		if (FAILED(pShader->Set_Matrix("g_LightProjMatrix", &pInst->Get_TransformFloat4x4(CPipeLine::D3DTS_LIGHTPROJ))))
			return E_FAIL;

		RELEASE_INSTANCE(CGameInstance);

		// 빛을 임시적으로 배치한 카메라라고 한번 생각해보기
		// 절두체 영역으로 보고 있을 것이고 물체가 그려져야할지 안 그려져야할지 
		// 깊이값 저장하는 것처럼
		// shadow를 그린다.

		iPassIndex = 1;
	}
	else if (LIGHTDESC::TYPE_POINT == m_LightDesc.eType)
	{
		if (FAILED(pShader->Set_RawValue("g_vLightPos", &m_LightDesc.vPosition, sizeof(_float4))))
			return E_FAIL;
		if (FAILED(pShader->Set_RawValue("g_fLightRange", &m_LightDesc.fRange, sizeof(_float))))
			return E_FAIL;

		iPassIndex = 2;
	}

	if (FAILED(pShader->Set_RawValue("g_vLightDiffuse", &m_LightDesc.vDiffuse, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vLightAmbient", &m_LightDesc.vAmbient, sizeof(_float4))))
		return E_FAIL;
	if (FAILED(pShader->Set_RawValue("g_vLightSpecular", &m_LightDesc.vSpecular, sizeof(_float4))))
		return E_FAIL;

	pShader->Begin(iPassIndex);

	pVIBuffer->Render();

	return S_OK;
}

CLight * CLight::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	CLight*		pInstance = new CLight(pDevice, pContext);

	if (FAILED(pInstance->Init(LightDesc)))
	{
		MSG_BOX("CLight Create Failed");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLight::Free()
{
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
