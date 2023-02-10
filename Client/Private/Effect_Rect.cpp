#include "stdafx.h"
#include "..\public\Effect_Rect.h"
#include "GameInstance.h"

CEffect_Rect::CEffect_Rect(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CEffect(pDevice, pContext)
{
}

CEffect_Rect::CEffect_Rect(const CEffect_Rect & rhs)
	: CEffect(rhs)
{
}

HRESULT CEffect_Rect::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CEffect_Rect::Init(void * pArg)
{
	CGameObject::GAMEOBJECTDESC		GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GameObjectDesc));

	GameObjectDesc.TransformDesc.fSpeedPerSec = 5.f;
	GameObjectDesc.TransformDesc.fRotationPerSec = XMConvertToRadians(90.0f);

	if (FAILED(__super::Init(&GameObjectDesc)))
		return E_FAIL;

	if (nullptr != pArg)
		memcpy(&m_RectEffectDesc, pArg, sizeof(m_RectEffectDesc));
	else
	{
		XMStoreFloat4x4(&m_RectEffectDesc.PivotMatrix, XMMatrixIdentity());
		m_RectEffectDesc.pTargetTransform = nullptr;
	}

	return S_OK;
}

void CEffect_Rect::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
}

void CEffect_Rect::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	__super::Compute_CamDistance();

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_ALPHABLEND, this);
}

HRESULT CEffect_Rect::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

void CEffect_Rect::Imgui_RenderProperty()
{
	ImGui::DragFloat("Alpha", &m_fAlpha, 0.1f, -10.f, 10.f);
	ImGui::DragFloat("MoveX", &m_UVMoveFactor.x, 0.1f, -10.f, 10.f);
	ImGui::DragFloat("MoveY", &m_UVMoveFactor.y, 0.1f, -10.f, 10.f);

	ImGui::Begin("Effect");
	m_pTransformCom->Imgui_RenderProperty();
	ImGui::End();
}

void CEffect_Rect::Compute_BillBoard()
{
	_mat BillBoardMatrix;
	CGameInstance* pGameInstance = GET_INSTANCE(CGameInstance);
	_float3 CamPosition = pGameInstance->Get_CamPosition();
	_float3	CamUp = pGameInstance->Get_CamUp();
	BillBoardMatrix = _mat::CreateBillboard(m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION), CamPosition, CamUp);
	m_pTransformCom->SetWorldMatrix(BillBoardMatrix);
	RELEASE_INSTANCE(CGameInstance);
}

void CEffect_Rect::Free()
{
	__super::Free();
	Safe_Release(m_pTextureCom);

	if(m_bClone)
		Safe_Release(m_RectEffectDesc.pTargetTransform);
	
	Safe_Release(m_pVIBufferCom);
}
