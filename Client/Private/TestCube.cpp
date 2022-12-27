#include "stdafx.h"
#include "..\Public\TestCube.h"
#include "GameInstance.h"
#include "MathUtils.h"

CTestCube::CTestCube(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CGameObject(pDevice, pContext)
{
}

CTestCube::CTestCube(const CTestCube & rhs)
	: CGameObject(rhs)
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
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_Scaled(_float3(0.3f, 0.3f, 0.3f));

	return S_OK;
}

void CTestCube::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
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

	m_pShaderCom->Begin(1);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CTestCube::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxCubeTex"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_VIBuffer_Cube"), TEXT("Com_VIBuffer"),
		(CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Texture_Sky"), TEXT("Com_Texture"),
		(CComponent**)&m_pTextureCom)))
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

	if (FAILED(m_pTextureCom->Bind_ShaderResource(m_pShaderCom, "g_Texture")))
		return E_FAIL;

	return S_OK;
}

_bool CTestCube::Peeking()
{
	POINT		ptMouse{};

	GetCursorPos(&ptMouse);
	ScreenToClient(g_hWnd, &ptMouse);

	_float3		fPoint;

	fPoint.x = _float(ptMouse.x / (g_iWinSizeX * 0.5f) - 1.f);
	fPoint.y = _float(ptMouse.y / -(g_iWinSizeY * 0.5f) + 1.f);
	fPoint.z = 1.f;

	_vector		vecPoint = XMLoadFloat3(&fPoint);

	vecPoint = XMVectorSetW(vecPoint, 1.f);

	CGameInstance*  pGameInstance = GET_INSTANCE(CGameInstance);

	_matrix		matProj;

	matProj = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_PROJ);

	matProj = XMMatrixInverse(nullptr, matProj);

	vecPoint = XMVector3TransformCoord(vecPoint, matProj);

	_float3 fRayPos = { 0.f, 0.f, 0.f };

	_vector		vecRayPos = XMLoadFloat3(&fRayPos);

	vecRayPos = XMVectorSetW(vecRayPos, 1.f);

	_vector		vecDir = vecPoint - vecRayPos;

	_matrix		matView;

	matView = pGameInstance->Get_TransformMatrix(CPipeLine::D3DTS_VIEW);

	matView = XMMatrixInverse(nullptr, matView);
	vecRayPos = XMVector3TransformCoord(vecRayPos, matView); // XMVector3TransformCoord입력 벡터의 w 구성요소를 무시하고 값 1.0을 대신 사용합니다. 반환된 벡터의 w 구성 요소는 항상 1.0입니다.
	vecDir = XMVector3TransformNormal(vecDir, matView); // XMVector3TransformNormal 는 회전 및 크기 조정을 위해 입력 행렬 행 0, 1 및 2를 사용하여 변환을 수행하고 행 3을 무시합니다.

	_matrix		matWorld = m_pTransformCom->Get_WorldMatrix();

	matWorld = XMMatrixInverse(nullptr, matWorld);
	vecRayPos = XMVector3TransformCoord(vecRayPos, matWorld);
	vecDir = XMVector3Normalize(XMVector3TransformNormal(vecDir, matWorld));

	_float4	fTest = { 0.f, 0.f, 0.f, 1.f };

	XMStoreFloat4(&fTest, vecRayPos);

	const _float4*	pCubeVtx = m_pVIBufferCom->Get_VtxPos();

	_float  fDist = 0.f;

	// +X //
	//  FXMVECTOR Origin, FXMVECTOR Direction, FXMVECTOR V0, GXMVECTOR V1, HXMVECTOR V2, float& Dist
	// Origin, Direction, Triangle initial vector 3 pack, cross distance
	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[1]),
		XMLoadFloat4(&pCubeVtx[5]),
		XMLoadFloat4(&pCubeVtx[6]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[1]),
		XMLoadFloat4(&pCubeVtx[6]),
		XMLoadFloat4(&pCubeVtx[2]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	// -X //
	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[4]),
		XMLoadFloat4(&pCubeVtx[0]),
		XMLoadFloat4(&pCubeVtx[3]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[4]),
		XMLoadFloat4(&pCubeVtx[3]),
		XMLoadFloat4(&pCubeVtx[7]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	// +Y //
	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[4]),
		XMLoadFloat4(&pCubeVtx[5]),
		XMLoadFloat4(&pCubeVtx[1]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[4]),
		XMLoadFloat4(&pCubeVtx[1]),
		XMLoadFloat4(&pCubeVtx[0]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	// -Y //

	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[3]),
		XMLoadFloat4(&pCubeVtx[2]),
		XMLoadFloat4(&pCubeVtx[6]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[3]),
		XMLoadFloat4(&pCubeVtx[6]),
		XMLoadFloat4(&pCubeVtx[7]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	// +Z //

	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[5]),
		XMLoadFloat4(&pCubeVtx[4]),
		XMLoadFloat4(&pCubeVtx[7]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[5]),
		XMLoadFloat4(&pCubeVtx[7]),
		XMLoadFloat4(&pCubeVtx[6]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	// -Z //

	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[0]),
		XMLoadFloat4(&pCubeVtx[1]),
		XMLoadFloat4(&pCubeVtx[2]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	if (TriangleTests::Intersects(vecRayPos, vecDir,
		XMLoadFloat4(&pCubeVtx[0]),
		XMLoadFloat4(&pCubeVtx[2]),
		XMLoadFloat4(&pCubeVtx[3]),
		fDist))
	{
		RELEASE_INSTANCE(CGameInstance);
		return true;
	}

	RELEASE_INSTANCE(CGameInstance);
	return false;
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

	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
