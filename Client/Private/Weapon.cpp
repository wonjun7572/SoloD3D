#include "stdafx.h"
#include "..\Public\Weapon.h"
#include "GameInstance.h"
#include "Bone.h"

CWeapon::CWeapon(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CWeapon::CWeapon(const CWeapon & rhs)
	: CGameObject(rhs)
{
}

HRESULT CWeapon::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::Init(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_WeaponDesc, pArg, sizeof(m_WeaponDesc));

	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL;

	m_pTransformCom->Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(0.15f, -0.85f, -2.015f, 1.f));
	
	return S_OK;
}

void CWeapon::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);

	//Imgui_RenderProperty();
}

void CWeapon::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);

	_matrix			SocketMatrix = 
		XMLoadFloat4x4(&m_WeaponDesc.pSocket->Get_OffsetMatrix()) * 
		XMLoadFloat4x4(&m_WeaponDesc.pSocket->Get_CombindMatrix()) * 
		XMLoadFloat4x4(&m_WeaponDesc.PivotMatrix);

	SocketMatrix.r[0] = XMVector3Normalize(SocketMatrix.r[0]);
	SocketMatrix.r[1] = XMVector3Normalize(SocketMatrix.r[1]);
	SocketMatrix.r[2] = XMVector3Normalize(SocketMatrix.r[2]);

	SocketMatrix = SocketMatrix * m_WeaponDesc.pTargetTransform->Get_WorldMatrix();

	XMStoreFloat4x4(&m_SocketMatrix, SocketMatrix);

	for (_uint i = 0; i < 5; ++i)
		m_pColliderCom[i]->Update(m_pTransformCom->Get_WorldMatrix() * SocketMatrix);

	if (nullptr != m_pRendererCom)
		m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONALPHABLEND, this);
}

HRESULT CWeapon::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	_uint iNumMeshes = m_pModelCom->Get_NumMeshes();

	for (_uint i = 0; i < iNumMeshes; ++i)
	{
		/* 이 모델을 그리기위한 셰이더에 머테리얼 텍스쳐를 전달하낟. */
		m_pModelCom->Bind_Material(m_pShaderCom, i, aiTextureType_DIFFUSE, "g_DiffuseTexture");

		m_pModelCom->Render(m_pShaderCom, i, 2);
	}

#ifdef _DEBUG
	for (_uint i = 0; i < 5; ++i)
	{
		m_pColliderCom[i]->Render();
	}
#endif 

	return S_OK;
}

HRESULT CWeapon::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Shader_VtxModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Sword"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom)))
		return E_FAIL;

	/* For.Com_Collider */
	CCollider::COLLIDERDESC			ColliderDesc;
	ZeroMemory(&ColliderDesc, sizeof(CCollider::COLLIDERDESC));

	ColliderDesc.vCenter = _float3(1.8f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.15f, 0.15f, 0.15f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_0"),
		(CComponent**)&m_pColliderCom[0], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.vCenter = _float3(1.5f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.15f, 0.15f, 0.15f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_1"),
		(CComponent**)&m_pColliderCom[1], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.vCenter = _float3(1.2f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.15f, 0.15f, 0.15f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_2"),
		(CComponent**)&m_pColliderCom[2], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.vCenter = _float3(0.9f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.15f, 0.15f, 0.15f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_3"),
		(CComponent**)&m_pColliderCom[3], &ColliderDesc)))
		return E_FAIL;

	ColliderDesc.vCenter = _float3(0.6f, 0.f, 0.f);
	ColliderDesc.vSize = _float3(0.15f, 0.15f, 0.15f);

	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Collider_SPHERE"), TEXT("Com_Collider_4"),
		(CComponent**)&m_pColliderCom[4], &ColliderDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CWeapon::SetUp_ShaderResources()
{
	if (nullptr == m_pShaderCom)
		return E_FAIL;

	// g_WorldMatrix * SocketMatrix;

	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = GET_INSTANCE(CGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &pGameInstance->Get_TransformFloat4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Set_Matrix("g_SocketMatrix", &m_SocketMatrix)))
		return E_FAIL;


	/* For.Lights */
	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	RELEASE_INSTANCE(CGameInstance);

	return S_OK;
}

void CWeapon::Imgui_RenderProperty()
{
	ImGui::Begin("Weapon");

	/*if (ImGui::CollapsingHeader("Transform!"))
	{
		ImGuizmo::BeginFrame();
		ImGui::InputFloat("SpeedPerSec", &m_pTransformCom->m_TransformDesc.fSpeedPerSec);
		ImGui::InputFloat("RotationPerSec", &m_pTransformCom->m_TransformDesc.fRotationPerSec);

		static float snap[3] = { 1.f, 1.f, 1.f };
		static ImGuizmo::OPERATION mCurrentGizmoOperation(ImGuizmo::ROTATE);
		static ImGuizmo::MODE mCurrentGizmoMode(ImGuizmo::WORLD);
		if (ImGui::IsKeyPressed(90))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		if (ImGui::IsKeyPressed(69))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		if (ImGui::IsKeyPressed(82)) // r Key
			mCurrentGizmoOperation = ImGuizmo::SCALE;
		if (ImGui::RadioButton("Translate", mCurrentGizmoOperation == ImGuizmo::TRANSLATE))
			mCurrentGizmoOperation = ImGuizmo::TRANSLATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Rotate", mCurrentGizmoOperation == ImGuizmo::ROTATE))
			mCurrentGizmoOperation = ImGuizmo::ROTATE;
		ImGui::SameLine();
		if (ImGui::RadioButton("Scale", mCurrentGizmoOperation == ImGuizmo::SCALE))
			mCurrentGizmoOperation = ImGuizmo::SCALE;

		float matrixTranslation[3], matrixRotation[3], matrixScale[3];

		ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&m_pTransformCom->m_WorldMatrix), matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Translate", matrixTranslation);
		ImGui::InputFloat3("Rotate", matrixRotation);
		ImGui::InputFloat3("Scale", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(&m_pTransformCom->m_WorldMatrix));

		if (mCurrentGizmoOperation != ImGuizmo::SCALE)
		{
			if (ImGui::RadioButton("Local", mCurrentGizmoMode == ImGuizmo::LOCAL))
				mCurrentGizmoMode = ImGuizmo::LOCAL;
			ImGui::SameLine();
			if (ImGui::RadioButton("World", mCurrentGizmoMode == ImGuizmo::WORLD))
				mCurrentGizmoMode = ImGuizmo::WORLD;
		}

		static bool useSnap(false);
		if (ImGui::IsKeyPressed(83))
			useSnap = !useSnap;
		ImGui::Checkbox("##something", &useSnap);
		ImGui::SameLine();
		switch (mCurrentGizmoOperation)
		{
		case ImGuizmo::TRANSLATE:
			ImGui::InputFloat3("Snap", &snap[0]);
			break;
		case ImGuizmo::ROTATE:
			ImGui::InputFloat("Angle Snap", &snap[0]);
			break;
		case ImGuizmo::SCALE:
			ImGui::InputFloat("Scale Snap", &snap[0]);
			break;
		}

		ImGuiIO& io = ImGui::GetIO();
		RECT rt;
		GetClientRect(CGameInstance::GetInstance()->GetHWND(), &rt);
		POINT lt{ rt.left, rt.top };
		ClientToScreen(CGameInstance::GetInstance()->GetHWND(), &lt);
		ImGuizmo::SetRect((_float)lt.x, (_float)lt.y, io.DisplaySize.x, io.DisplaySize.y);

		_float4x4 matView, matProj;
		XMStoreFloat4x4(&matView, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
		XMStoreFloat4x4(&matProj, CGameInstance::GetInstance()->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

		ImGuizmo::Manipulate(
			reinterpret_cast<float*>(&matView),
			reinterpret_cast<float*>(&matProj),
			mCurrentGizmoOperation,
			mCurrentGizmoMode,
			reinterpret_cast<float*>(&m_pTransformCom->m_WorldMatrix),
			nullptr, useSnap ? &snap[0] : nullptr);
	}*/
	m_pColliderCom[1]->FixedSize();

	ImGui::End();
}

CWeapon * CWeapon::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CWeapon*		pInstance = new CWeapon(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CWeapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CWeapon::Clone(void * pArg)
{
	CWeapon*		pInstance = new CWeapon(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWeapon");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CWeapon::Free()
{
	__super::Free();

	if (true == m_bClone)
	{
		Safe_Release(m_WeaponDesc.pSocket);
		Safe_Release(m_WeaponDesc.pTargetTransform);
	}
	for (_uint i = 0; i < 5; ++i)
		Safe_Release(m_pColliderCom[i]);
	
	Safe_Release(m_pModelCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
}
