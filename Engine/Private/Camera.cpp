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
	/* ī�޶��� Ʈ���������� ���� ��������� �����ͼ� ������� ���Ѵ���(������� ���ߴ�.) ���������ο� �����Ѵ�. */
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
	// TODO: Camera aspect, fov, near, far ���� �Ҽ��ֵ���

}

void CCamera::Free()
{
	__super::Free();

	Safe_Release(m_pPipeLine);
}
