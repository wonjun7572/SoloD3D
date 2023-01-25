#include "..\Public\Transform.h"
#include "Shader.h"
#include "GameInstance.h"
#include "Navigation.h"

CTransform::CTransform(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CTransform::CTransform(const CTransform & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
{
}

void CTransform::Set_Scaled(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return;

	_vector vState = Get_State(eState);
	vState = XMVector3Normalize(vState) * fScale;
	Set_State(eState, vState);
}

void CTransform::Set_Scaled(_float3 vScale)
{
	Set_State(CTransform::STATE_RIGHT, XMVector3Normalize(Get_State(STATE_RIGHT)) * vScale.x);
	Set_State(CTransform::STATE_UP, XMVector3Normalize(Get_State(STATE_UP)) * vScale.y);
	Set_State(CTransform::STATE_LOOK, XMVector3Normalize(Get_State(STATE_LOOK)) * vScale.z);
}

void CTransform::Scaling(STATE eState, _float fScale)
{
	if (eState == STATE_TRANSLATION)
		return;

	Set_State(eState, Get_State(eState) * fScale);
}

HRESULT CTransform::Init_Prototype()
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	return S_OK;
}

HRESULT CTransform::Init(void * pArg)
{
	if (nullptr != pArg)
		memcpy(&m_TransformDesc, pArg, sizeof(TRANSFORMDESC));

	return S_OK;
}

void CTransform::Imgui_RenderProperty()
{
	if (ImGui::CollapsingHeader("Transform!"))
	{
		ImGuizmo::BeginFrame();
		ImGui::InputFloat("SpeedPerSec", &m_TransformDesc.fSpeedPerSec);
		ImGui::InputFloat("RotationPerSec", &m_TransformDesc.fRotationPerSec);

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

		ImGuizmo::DecomposeMatrixToComponents(reinterpret_cast<float*>(&m_WorldMatrix), matrixTranslation, matrixRotation, matrixScale);
		ImGui::InputFloat3("Translate", matrixTranslation);
		ImGui::InputFloat3("Rotate", matrixRotation);
		ImGui::InputFloat3("Scale", matrixScale);
		ImGuizmo::RecomposeMatrixFromComponents(matrixTranslation, matrixRotation, matrixScale, reinterpret_cast<float*>(&m_WorldMatrix));

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
			reinterpret_cast<float*>(&m_WorldMatrix),
			nullptr, useSnap ? &snap[0] : nullptr);
	}
}

void CTransform::Go_Straight(_double TimeDelta,CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition += XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * static_cast<float>(TimeDelta);

	if (nullptr == pNaviCom)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);

	else
	{
		_float4 vOldPos = Get_State(CTransform::STATE_TRANSLATION);
		if (true == pNaviCom->isMove_OnNavigation(vPosition, &vOldPos))
		{
			Set_State(CTransform::STATE_TRANSLATION, vPosition);
		}
		else
		{
			Set_State(CTransform::STATE_TRANSLATION, vOldPos);
		}
	}
}

void CTransform::Go_Backward(_double TimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vLook = Get_State(CTransform::STATE_LOOK);

	vPosition -= XMVector3Normalize(vLook) * m_TransformDesc.fSpeedPerSec * static_cast<float>(TimeDelta);

	if (nullptr == pNaviCom)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);
	else
	{
		_float4 vOldPos = Get_State(CTransform::STATE_TRANSLATION);
		if (true == pNaviCom->isMove_OnNavigation(vPosition, &vOldPos))
		{
			Set_State(CTransform::STATE_TRANSLATION, vPosition);
		}
		else
		{
			Set_State(CTransform::STATE_TRANSLATION, vOldPos);
		}
	}
}

void CTransform::Go_Left(_double TimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition -= XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * static_cast<float>(TimeDelta);

	if (nullptr == pNaviCom)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);

	else
	{
		_float4 vOldPos = Get_State(CTransform::STATE_TRANSLATION);
		if (true == pNaviCom->isMove_OnNavigation(vPosition, &vOldPos))
		{
			Set_State(CTransform::STATE_TRANSLATION, vPosition);
		}
		else
		{
			Set_State(CTransform::STATE_TRANSLATION, vOldPos);
		}
	}
}

void CTransform::Go_Right(_double TimeDelta, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vRight = Get_State(CTransform::STATE_RIGHT);

	vPosition += XMVector3Normalize(vRight) * m_TransformDesc.fSpeedPerSec * static_cast<float>(TimeDelta);

	if (nullptr == pNaviCom)
		Set_State(CTransform::STATE_TRANSLATION, vPosition);

	else
	{
		_float4 vOldPos = Get_State(CTransform::STATE_TRANSLATION);
		if (true == pNaviCom->isMove_OnNavigation(vPosition, &vOldPos))
		{
			Set_State(CTransform::STATE_TRANSLATION, vPosition);
		}
		else
		{
			Set_State(CTransform::STATE_TRANSLATION, vOldPos);
		}
	}
}

void CTransform::Go_Up(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vUP = Get_State(CTransform::STATE_UP);

	vPosition += XMVector3Normalize(vUP) * m_TransformDesc.fSpeedPerSec  * static_cast<float>(TimeDelta);

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Down(_double TimeDelta)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vUP = Get_State(CTransform::STATE_UP);

	vPosition -= XMVector3Normalize(vUP) * m_TransformDesc.fSpeedPerSec  * static_cast<float>(TimeDelta);

	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

void CTransform::Go_Direction(_double TimeDelta, _float3 vDir)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	vPosition += XMVector3Normalize(vDir)* m_TransformDesc.fSpeedPerSec  * static_cast<float>(TimeDelta);
	Set_State(CTransform::STATE_TRANSLATION, vPosition);
}

_bool CTransform::Jump(_double TimeDelta, _float fJumHeight, _float fLandHeight)
{
	static _float fGravity = 8.81f;
	_float4 vPos;
	XMStoreFloat4(&vPos, Get_State(CTransform::STATE_TRANSLATION));

	_float fVelocity = sqrtf(fabsf(fJumHeight * 2.f * fGravity));

	if (vPos.y >= fVelocity)
		fGravity *= -1.f;

	m_WorldMatrix._42 = max(vPos.y + fGravity * (float)TimeDelta, fLandHeight);

	if (m_WorldMatrix._42 - fLandHeight <= 0.00001f)
	{
		fGravity *= -1.f;
		return true;
	}

	return false;
}

void CTransform::Turn(_fvector vAxis, _double TimeDelta)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, m_TransformDesc.fRotationPerSec * static_cast<float>(TimeDelta));

	Set_State(CTransform::STATE_RIGHT, XMVector3TransformNormal(Get_State(CTransform::STATE_RIGHT), RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector3TransformNormal(Get_State(CTransform::STATE_UP), RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector3TransformNormal(Get_State(CTransform::STATE_LOOK), RotationMatrix));
}

void CTransform::Rotation(_fvector vAxis, _float fRadian)
{
	_matrix		RotationMatrix = XMMatrixRotationAxis(vAxis, fRadian);

	_float3		vScale = Get_Scaled();

	_vector		vRight = XMVectorSet(1.f, 0.f, 0.f, 0.f) * vScale.x;
	_vector		vUp = XMVectorSet(0.f, 1.f, 0.f, 0.f) * vScale.y;
	_vector		vLook = XMVectorSet(0.f, 0.f, 1.f, 0.f) * vScale.z;

	Set_State(CTransform::STATE_RIGHT, XMVector4Transform(vRight, RotationMatrix));
	Set_State(CTransform::STATE_UP, XMVector4Transform(vUp, RotationMatrix));
	Set_State(CTransform::STATE_LOOK, XMVector4Transform(vLook, RotationMatrix));
}

void CTransform::LookAt(_fvector vTargetPos, _bool bY)
{
	_float3   vScale = Get_Scaled();
	if (bY == false)
	{
		_vector   vPosWithoutY = XMVectorSetY(vTargetPos, m_WorldMatrix._42);
		_vector   vLook = XMVector3Normalize(vPosWithoutY - Get_State(STATE_TRANSLATION)) * vScale.z;
		_vector   vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
		_vector   vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

		Set_State(STATE_RIGHT, vRight);
		Set_State(STATE_UP, vUp);
		Set_State(STATE_LOOK, vLook);
	}
	else
	{
		_vector		vLook = XMVector3Normalize(vTargetPos - Get_State(CTransform::STATE_TRANSLATION)) * vScale.z;
		_vector		vRight = XMVector3Normalize(XMVector3Cross(XMVectorSet(0.f, 1.f, 0.f, 0.f), vLook)) * vScale.x;
		_vector		vUp = XMVector3Normalize(XMVector3Cross(vLook, vRight)) * vScale.y;

		Set_State(CTransform::STATE_RIGHT, vRight);
		Set_State(CTransform::STATE_UP, vUp);
		Set_State(CTransform::STATE_LOOK, vLook);
	}
}

void CTransform::Chase(_fvector vTargetPos, _double TimeDelta, _float fLimit, CNavigation* pNaviCom)
{
	_vector		vPosition = Get_State(CTransform::STATE_TRANSLATION);
	_vector		vDir = vTargetPos - vPosition;

	_float		fDistance = XMVectorGetX(XMVector3Length(vDir));

	if (fDistance > fLimit)
	{
		vPosition += XMVector3Normalize(vDir) * m_TransformDesc.fSpeedPerSec * static_cast<float>(TimeDelta);
		if (nullptr == pNaviCom)
			Set_State(CTransform::STATE_TRANSLATION, vPosition);
		else
		{
			_float4 vOldPos = Get_State(CTransform::STATE_TRANSLATION);
			if (true == pNaviCom->isMove_OnNavigation(vPosition, &vOldPos))
			{
				Set_State(CTransform::STATE_TRANSLATION, vPosition);
			}
			else
			{
				Set_State(CTransform::STATE_TRANSLATION, vOldPos);
			}
		}
	}
}

void CTransform::ChaseAndLookAt(_fvector vTargetPos, _double TimeDelta, _float fLimit, CNavigation * pNaviCom)
{
	LookAt(vTargetPos);
	Chase(vTargetPos, TimeDelta, fLimit, pNaviCom);
}

void CTransform::Set_Height(_float Ypos)
{
	_float4		vPos;
	XMStoreFloat4(&vPos, Get_State(CTransform::STATE_TRANSLATION));
	Set_State(CTransform::STATE_TRANSLATION, XMVectorSet(vPos.x, Ypos, vPos.z, 1.f));
}

HRESULT CTransform::Bind_ShaderResource(CShader * pShaderCom, const char * pConstantName)
{
	if (nullptr == pShaderCom)
		return E_FAIL;

	return pShaderCom->Set_Matrix(pConstantName, &m_WorldMatrix);
}

CTransform * CTransform::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CTransform*	pInstance = new CTransform(pDevice, pContext);

	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed to Created : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent * CTransform::Clone(void * pArg)
{
	CTransform*	pInstance = new CTransform(*this);

	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed to Cloned : CTransform");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTransform::Free()
{
	__super::Free();
}
