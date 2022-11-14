#include "stdafx.h"
#include "..\Public\TestCube.h"
#include "GameInstance.h"
#include "MathUtils.h"

CTestCube::CTestCube(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CTestCube::CTestCube(const CTestCube & rhs)
	:CGameObject(rhs)
{
}

HRESULT CTestCube::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestCube::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Go_Right(0.2);
	
	_float4 i;
	XMStoreFloat4(&i, m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION));
	_vector v;
	_float4 k;
	k = _float4(1.f, 1.f, 1.f, 1.f);
	v = XMLoadFloat4(&k);
	_float4 j;
	XMStoreFloat4(&j, v);
	
	_float b = CMathUtils::Distance(i, j);

	_float4 result;

	result = CMathUtils::Max(i, j);

	result;

	return S_OK;
}

void CTestCube::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	m_fTimeDelta += 0.001f;
	if (m_fTimeDelta > 1.f)
		m_fTimeDelta = 0.f;

	XMVECTOR v = XMLoadFloat4(&CMathUtils::Hermite(_float4(0.f, 0.f, 0.f, 1.f),
		_float4(10.f, 10.f, 10.f, 0.f), _float4(0.f, 0.f, 0.f, 1.f), _float4(20.f, 20.f, 20.f, 0.f), m_fTimeDelta));

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, v);
}

void CTestCube::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CTestCube::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTestCube::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	ZeroMemory(&TransformDesc, sizeof(CTransform::TRANSFORMDESC));

	TransformDesc.fSpeedPerSec = 5.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"), TEXT("Com_Transform"),
		(CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;


	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CTestCube::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &m_pTransformCom->Get_World4x4())))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

CTestCube * CTestCube::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTestCube*		pInstance = new CTestCube(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CTestCube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CTestCube::Clone(void * pArg)
{
	CTestCube*		pInstance = new CTestCube(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTestCube");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CTestCube::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
