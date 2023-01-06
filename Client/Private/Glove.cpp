#include "stdafx.h"
#include "..\Public\Glove.h"
#include "GameInstance.h"
#include "Bone.h"

CGlove::CGlove(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CParts(pDevice, pContext)
{
}

CGlove::CGlove(const CGlove & rhs)
	: CParts(rhs)
{
}

HRESULT CGlove::Init_Prototype()
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlove::Init(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	if (FAILED(SetUp_Components()))
		return E_FAIL; 

	return S_OK;
}

void CGlove::Tick(_double TimeDelta)
{
	__super::Tick(TimeDelta);
	m_pModelCom[m_eModelState]->Set_AnimationIndex(m_iAnimIndex);
	m_pModelCom[m_eModelState]->Play_Animation(TimeDelta);
	//Imgui_RenderProperty();
}

void CGlove::Late_Tick(_double TimeDelta)
{
	__super::Late_Tick(TimeDelta);
}

HRESULT CGlove::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CGlove::SetUp_Components()
{
	/* For.Com_Renderer */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Renderer"), TEXT("Com_Renderer"),
		(CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(__super::Add_Component(CGameInstance::Get_StaticLevelIndex(), TEXT("Prototype_Component_Shader_VtxAnimModel"), TEXT("Com_Shader"),
		(CComponent**)&m_pShaderCom)))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Glove"), TEXT("Com_Model"),
		(CComponent**)&m_pModelCom[MODEL_NOMAL])))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Glove_A"), TEXT("Com_Model_A"),
		(CComponent**)&m_pModelCom[MODEL_A])))
		return E_FAIL;

	/* For.Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_CHAP1, TEXT("Prototype_Component_Model_Glove_B"), TEXT("Com_Model_B"),
		(CComponent**)&m_pModelCom[MODEL_B])))
		return E_FAIL;

	return S_OK;
}

void CGlove::Imgui_RenderProperty()
{
	ImGui::Begin("Upper");

	if (ImGui::CollapsingHeader("Transform!"))
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
	}

	ImGui::End();
}

CGlove * CGlove::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CGlove*		pInstance = new CGlove(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CGlove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CGameObject * CGlove::Clone(void * pArg)
{
	CGlove*		pInstance = new CGlove(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CGlove");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CGlove::Free()
{
	__super::Free();
}
