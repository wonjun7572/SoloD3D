#include "..\Public\Camera.h"

#include "PipeLine.h"

CCamera::CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_pPipeLine(CPipeLine::GetInstance())
{
	Safe_AddRef(m_pPipeLine);
}

CCamera::CCamera(const CCamera & rhs)
	: CGameObject(rhs)
	, m_pPipeLine(rhs.m_pPipeLine)
{
	Safe_AddRef(m_pPipeLine);
}

HRESULT CCamera::Init_Prototype()
{
	return S_OK;
}

HRESULT CCamera::Init(void * pArg)
{
	ZeroMemory(&m_CameraDesc, sizeof(CAMERADESC));

	if (pArg != nullptr)
		memcpy(&m_CameraDesc, pArg, sizeof(m_CameraDesc));

	if (FAILED(__super::Init(&m_CameraDesc)))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMLoadFloat4(&m_CameraDesc.vEye));
	m_pTransformCom->LookAt(XMLoadFloat4(&m_CameraDesc.vAt));

	return S_OK;
}

void CCamera::Tick(_double fTimeDelta)
{
	if (nullptr == m_pPipeLine)
		return;
	/* 카메라의 트랜스폼으로 부터 월드행렬을 가져와서 역행렬을 구한다음(뷰행렬을 구했다.) 파이프라인에 저장한다. */
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_VIEW, m_pTransformCom->Get_WorldMatrixInverse());
	m_pPipeLine->Set_Transform(CPipeLine::D3DTS_PROJ, XMMatrixPerspectiveFovLH(m_CameraDesc.fFovy, m_CameraDesc.fAspect, m_CameraDesc.fNear, m_CameraDesc.fFar));
}

void CCamera::Late_Tick(_double fTimeDelta)
{
}

HRESULT CCamera::Render()
{
	return S_OK;
}

void CCamera::Imgui_RenderProperty()
{
	// TODO: Camera aspect, fov, near, far 세팅 할수있도록

}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
}
