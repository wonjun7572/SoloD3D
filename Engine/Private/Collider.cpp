#include "..\public\Collider.h"
#include "DebugDraw.h"

#include "PipeLine.h"

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{

}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_eType(rhs.m_eType)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif // _DEBUG

}

HRESULT CCollider::Initialize_Prototype(TYPE eType)
{
	m_eType = eType;

#ifdef _DEBUG

	m_pBatch = new PrimitiveBatch<VertexPositionColor>(m_pContext);

	m_pEffect = new BasicEffect(m_pDevice);
	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode;
	size_t			iShaderByteCodeSize;

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeSize);

	if (FAILED(m_pDevice->CreateInputLayout(VertexPositionColor::InputElements, VertexPositionColor::InputElementCount, pShaderByteCode, iShaderByteCodeSize, &m_pInputLayout)))
		return E_FAIL;

#endif

	return S_OK;
}

HRESULT CCollider::Initialize(void * pArg)
{
	COLLIDERDESC		ColliderDesc;
	memcpy(&ColliderDesc, pArg, sizeof(COLLIDERDESC));

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB_Original = new BoundingBox(_float3(0.f, 0.f, 0.f), _float3(1.f, 1.f, 1.f));
		m_pAABB_Original->Transform(*m_pAABB_Original,
			XMMatrixScaling(ColliderDesc.vSize.x, ColliderDesc.vSize.y, ColliderDesc.vSize.z) *
			XMMatrixTranslation(ColliderDesc.vCenter.x, ColliderDesc.vCenter.y, ColliderDesc.vCenter.z));
		m_pAABB = new BoundingBox(*m_pAABB_Original);
		break; 

	case CCollider::TYPE_OBB:
		m_pOBB_Original = new BoundingOrientedBox(_float3(0.f, 0.f, 0.f), _float3(ColliderDesc.vSize.x, ColliderDesc.vSize.y, ColliderDesc.vSize.z), _float4(0.f, 0.f, 0.f, 1.f));
		m_pOBB_Original->Transform(*m_pOBB_Original,
			XMMatrixRotationX(ColliderDesc.vRotation.x) *
			XMMatrixRotationY(ColliderDesc.vRotation.y) *
			XMMatrixRotationZ(ColliderDesc.vRotation.z) *
			XMMatrixTranslation(ColliderDesc.vCenter.x, ColliderDesc.vCenter.y, ColliderDesc.vCenter.z));
		m_pOBB = new BoundingOrientedBox(*m_pOBB_Original);
		break;

	case CCollider::TYPE_SPHERE:
		m_pSphere_Original = new BoundingSphere(_float3(0.f, 0.f, 0.f), 1.f);

		m_pSphere_Original->Transform(*m_pSphere_Original,
			XMMatrixScaling(ColliderDesc.vSize.x, ColliderDesc.vSize.y, ColliderDesc.vSize.z) *
			XMMatrixRotationX(ColliderDesc.vRotation.x) *
			XMMatrixRotationY(ColliderDesc.vRotation.y) *
			XMMatrixRotationZ(ColliderDesc.vRotation.z) *
			XMMatrixTranslation(ColliderDesc.vCenter.x, ColliderDesc.vCenter.y, ColliderDesc.vCenter.z));

		m_pSphere = new BoundingSphere(*m_pSphere_Original);
		break;
	}

	return S_OK;
}

_float3 CCollider::Get_CollisionCenter()
{
	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		return m_pAABB->Center;
		break;

	case CCollider::TYPE_OBB:
		return m_pOBB->Center;
		break;

	case CCollider::TYPE_SPHERE:
		return m_pSphere->Center;
		break;
	}

	return _float3();
}

_float CCollider::Get_SphereRadius()
{
	if (m_pSphere == nullptr)
		return 0.f;

	return m_pSphere->Radius;
}

void CCollider::Update(_fmatrix TransformMatrix)
{
	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		m_pAABB_Original->Transform(*m_pAABB, Remove_Rotation(TransformMatrix));
		break;

	case CCollider::TYPE_OBB:
		m_pOBB_Original->Transform(*m_pOBB, TransformMatrix);
		break;

	case CCollider::TYPE_SPHERE:
		m_pSphere_Original->Transform(*m_pSphere, TransformMatrix);
		break;
	}
}

_bool CCollider::Collision(CCollider * pTargetCollider)
{
	m_isColl = false;

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_isColl = m_pAABB->Intersects(*pTargetCollider->m_pAABB);
		if (TYPE_OBB == pTargetCollider->m_eType)
			m_isColl = m_pAABB->Intersects(*pTargetCollider->m_pOBB);
		if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_isColl = m_pAABB->Intersects(*pTargetCollider->m_pSphere);
		break;
	case CCollider::TYPE_OBB:
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_isColl = m_pOBB->Intersects(*pTargetCollider->m_pAABB);
		if (TYPE_OBB == pTargetCollider->m_eType)
			m_isColl = m_pOBB->Intersects(*pTargetCollider->m_pOBB);
		if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_isColl = m_pOBB->Intersects(*pTargetCollider->m_pSphere);
		break;
	case CCollider::TYPE_SPHERE:
		if (TYPE_AABB == pTargetCollider->m_eType)
			m_isColl = m_pSphere->Intersects(*pTargetCollider->m_pAABB);
		if (TYPE_OBB == pTargetCollider->m_eType)
			m_isColl = m_pSphere->Intersects(*pTargetCollider->m_pOBB);
		if (TYPE_SPHERE == pTargetCollider->m_eType)
			m_isColl = m_pSphere->Intersects(*pTargetCollider->m_pSphere);
		break;
	}

	return m_isColl;
}

_bool CCollider::Collision_AABB(CCollider * pTargetCollider)
{
	if (TYPE_AABB != m_eType ||
		TYPE_AABB != pTargetCollider->m_eType)
		return false;

	_float3			vSourMin, vSourMax;
	_float3			vDestMin, vDestMax;

	vSourMin = Compute_Min();
	vSourMax = Compute_Max();
	vDestMin = pTargetCollider->Compute_Min();
	vDestMax = pTargetCollider->Compute_Max();

	m_isColl = false;

	/* x�� ���󿡼� ��. */
	if (max(vSourMin.x, vDestMin.x) > min(vSourMax.x, vDestMax.x))
		return m_isColl;

	/* y�� ���󿡼� ��. */
	if (max(vSourMin.y, vDestMin.y) > min(vSourMax.y, vDestMax.y))
		return m_isColl;

	/* z�� ���󿡼� ��. */
	if (max(vSourMin.z, vDestMin.z) > min(vSourMax.z, vDestMax.z))
		return m_isColl;

	m_isColl = true;

	return m_isColl;
}

_bool CCollider::Collision_OBB(CCollider * pTargetCollider)
{
	if (TYPE_OBB != m_eType ||
		TYPE_OBB != pTargetCollider->m_eType)
		return false;

	m_isColl = true;

	OBBDESC		OBBDesc[2];

	OBBDesc[0] = Compute_OBBDesc();
	OBBDesc[1] = pTargetCollider->Compute_OBBDesc();

	/* 0 : �� �ڽ��� ���͸� �մ� ���͸� ������Ų ����.  */
	/* 1 : ù��° �ڽ��� CenterAxis������ ������Ų ������ ��.  */
	/* 2 : �ι�° �ڽ��� CenterAxis������ ������Ų ������ ��.  */
	_float			fDistance[3];

	for (_uint i = 0; i < 2; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
				XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[0]),
				XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[1]),
					XMLoadFloat3(&OBBDesc[i].vAlignAxis[j])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[2]),
					XMLoadFloat3(&OBBDesc[i].vAlignAxis[j]))));

			fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[0]),
				XMLoadFloat3(&OBBDesc[i].vAlignAxis[0])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[1]),
					XMLoadFloat3(&OBBDesc[i].vAlignAxis[0])))) +
				fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[2]),
					XMLoadFloat3(&OBBDesc[i].vAlignAxis[0]))));

			if (fDistance[0] > fDistance[1] + fDistance[2])
			{
				m_isColl = false;
				return m_isColl;
			}
		}
	}

	_vector			vAlignAxis[9];

	for (_uint i = 0; i < 3; ++i)
	{
		for (_uint j = 0; j < 3; ++j)
		{
			_uint		iIndex = i * 3 + j;

			vAlignAxis[iIndex] = XMVector3Cross(XMLoadFloat3(&OBBDesc[0].vAlignAxis[i]), XMLoadFloat3(&OBBDesc[1].vAlignAxis[j]));
		}
}

	for (_uint i = 0; i < 9; ++i)
	{
		fDistance[0] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenter) - XMLoadFloat3(&OBBDesc[0].vCenter),
			vAlignAxis[i])));

		fDistance[1] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[0]),
			vAlignAxis[i]))) +
			fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[1]),
				vAlignAxis[i]))) +
			fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[0].vCenterAxis[2]),
				vAlignAxis[i])));

		fDistance[2] = fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[0]),
			vAlignAxis[i]))) +
			fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[1]),
				vAlignAxis[i]))) +
			fabs(XMVectorGetX(XMVector3Dot(XMLoadFloat3(&OBBDesc[1].vCenterAxis[2]),
				vAlignAxis[i])));

		if (fDistance[0] > fDistance[1] + fDistance[2])
		{
			m_isColl = false;
			return m_isColl;
		}
	}

	return m_isColl;
}

void CCollider::FixedSphereSize(const _float& fX, const _float& fY, const _float& fZ, const _float& fSize)
{
	if (m_pSphere != nullptr)
	{
		m_pSphere_Original->Center.x = fX;
		m_pSphere_Original->Center.y = fY;
		m_pSphere_Original->Center.z = fZ;
		m_pSphere_Original->Radius = fSize;
	}
}

void CCollider::FixedSizeForImgui(_uint iIndex)
{
	if (iIndex == 0)
	{
		if (ImGui::CollapsingHeader("AABB"))
		{
			if (m_pAABB != nullptr)
			{
				ImGui::DragFloat("X", &m_X, 0.01f, -50.f, 50.f);
				ImGui::DragFloat("Y", &m_Y, 0.01f, -50.f, 50.f);
				ImGui::DragFloat("Z", &m_Z, 0.01f, -50.f, 50.f);

				ImGui::DragFloat("CX", &m_CX, 0.01f, 0.f, 50.f);
				ImGui::DragFloat("CY", &m_CY, 0.01f, 0.f, 50.f);
				ImGui::DragFloat("CZ", &m_CZ, 0.01f, 0.f, 50.f);

				m_pAABB_Original->Center.x = m_X;
				m_pAABB_Original->Center.y = m_Y;
				m_pAABB_Original->Center.z = m_Z;
				m_pAABB_Original->Extents.x = m_CX;
				m_pAABB_Original->Extents.y = m_CY;
				m_pAABB_Original->Extents.z = m_CZ;

				ImGui::Text("X: %f", m_pAABB_Original->Center.x); ImGui::SameLine();
				ImGui::Text("Y: %f", m_pAABB_Original->Center.y); ImGui::SameLine();
				ImGui::Text("Z: %f", m_pAABB_Original->Center.z);
				ImGui::Text("CX: %f", m_pAABB_Original->Extents.x); ImGui::SameLine();
				ImGui::Text("CY: %f", m_pAABB_Original->Extents.y); ImGui::SameLine();
				ImGui::Text("CZ: %f", m_pAABB_Original->Extents.z);
			}
		}
	}

	if (iIndex == 1)
	{
		if (ImGui::CollapsingHeader("Sphere"))
		{
			if (m_pSphere != nullptr)
			{
				ImGui::DragFloat("Sphere_X", &m_SphereX, 0.01f, -50.f, 50.f);
				ImGui::DragFloat("Sphere_Y", &m_SphereY, 0.01f, -50.f, 50.f);
				ImGui::DragFloat("Sphere_Z", &m_SphereZ, 0.01f, -50.f, 50.f);

				ImGui::DragFloat("Sphere_CX", &m_fRadius, 0.01f, 0.f, 50.f);

				m_pSphere_Original->Center.x = m_SphereX;
				m_pSphere_Original->Center.y = m_SphereY;
				m_pSphere_Original->Center.z = m_SphereZ;
				m_pSphere_Original->Radius = m_fRadius;

				ImGui::Text("Sphere_X: %f", m_pSphere_Original->Center.x); ImGui::SameLine();
				ImGui::Text("Sphere_Y: %f", m_pSphere_Original->Center.y); ImGui::SameLine();
				ImGui::Text("Sphere_Z: %f", m_pSphere_Original->Center.z);
				ImGui::Text("Sphere_Raduis: %f", m_pSphere_Original->Radius);
			}
		}
	}

	if (iIndex == 2)
	{
		if (ImGui::CollapsingHeader("OBB"))
		{
			if (m_pOBB != nullptr)
			{
				ImGui::DragFloat("OBB_X", &m_OBBX, 0.01f, -50.f, 50.f);
				ImGui::DragFloat("OBB_Y", &m_OBBY, 0.01f, -50.f, 50.f);
				ImGui::DragFloat("OBB_Z", &m_OBBZ, 0.01f, -50.f, 50.f);

				ImGui::DragFloat("OBB_CX", &m_OBBCX, 0.01f, 0.f, 50.f);
				ImGui::DragFloat("OBB_CY", &m_OBBCY, 0.01f, 0.f, 50.f);
				ImGui::DragFloat("OBB_CZ", &m_OBBCZ, 0.01f, 0.f, 50.f);

				m_pOBB_Original->Center.x = m_OBBX;
				m_pOBB_Original->Center.y = m_OBBY;
				m_pOBB_Original->Center.z = m_OBBZ;
				m_pOBB_Original->Extents.x = m_OBBCX;
				m_pOBB_Original->Extents.y = m_OBBCY;
				m_pOBB_Original->Extents.z = m_OBBCZ;

				ImGui::Text("OBB_X: %f", m_pOBB_Original->Center.x); ImGui::SameLine();
				ImGui::Text("OBB_Y: %f", m_pOBB_Original->Center.y); ImGui::SameLine();
				ImGui::Text("OBB_Z: %f", m_pOBB_Original->Center.z);
				ImGui::Text("OBB_CX: %f", m_pOBB_Original->Extents.x); ImGui::SameLine();
				ImGui::Text("OBB_CY: %f", m_pOBB_Original->Extents.y); ImGui::SameLine();
				ImGui::Text("OBB_CZ: %f", m_pOBB_Original->Extents.z);
			}
		}
	}
}

#ifdef _DEBUG

HRESULT CCollider::Render()
{
	m_vColor = m_isColl == true ? _float4(1.f, 0.f, 0.f, 1.f) : _float4(0.f, 1.f, 0.f, 1.f);

	m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	m_pEffect->SetView(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_VIEW));
	m_pEffect->SetProjection(pPipeLine->Get_TransformMatrix(CPipeLine::D3DTS_PROJ));

	RELEASE_INSTANCE(CPipeLine);

	m_pEffect->Apply(m_pContext);

	m_pContext->IASetInputLayout(m_pInputLayout);

	m_pContext->GSSetShader(nullptr,nullptr, 0);

	m_pBatch->Begin();

	switch (m_eType)
	{
	case CCollider::TYPE_AABB:
		DX::Draw(m_pBatch, *m_pAABB, XMLoadFloat4(&m_vColor));
		break;
	case CCollider::TYPE_OBB:
		DX::Draw(m_pBatch, *m_pOBB, XMLoadFloat4(&m_vColor));
		break;
	case CCollider::TYPE_SPHERE:
		DX::Draw(m_pBatch, *m_pSphere, XMLoadFloat4(&m_vColor));
		break;
	}

	m_pBatch->End();

	return S_OK;
}
#endif // _DEBUG

_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix			ResultMatrix = TransformMatrix;

	ResultMatrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[0]));
	ResultMatrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[1]));
	ResultMatrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(TransformMatrix.r[2]));

	return ResultMatrix;
}

_float3 CCollider::Compute_Min()
{
	return _float3(m_pAABB->Center.x - m_pAABB->Extents.x,
		m_pAABB->Center.y - m_pAABB->Extents.y,
		m_pAABB->Center.z - m_pAABB->Extents.z);
}

_float3 CCollider::Compute_Max()
{
	return _float3(m_pAABB->Center.x + m_pAABB->Extents.x,
		m_pAABB->Center.y + m_pAABB->Extents.y,
		m_pAABB->Center.z + m_pAABB->Extents.z);
}

CCollider::OBBDESC CCollider::Compute_OBBDesc()
{
	OBBDESC			OBBDesc;
	ZeroMemory(&OBBDesc, sizeof OBBDesc);

	OBBDesc.vCenter = m_pOBB->Center;

	_float3		vCorner[8];

	m_pOBB->GetCorners(vCorner);

	XMStoreFloat3(&OBBDesc.vCenterAxis[0], (XMLoadFloat3(&vCorner[2]) - XMLoadFloat3(&vCorner[3])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterAxis[1], (XMLoadFloat3(&vCorner[2]) - XMLoadFloat3(&vCorner[1])) * 0.5f);
	XMStoreFloat3(&OBBDesc.vCenterAxis[2], (XMLoadFloat3(&vCorner[2]) - XMLoadFloat3(&vCorner[6])) * 0.5f);

	XMStoreFloat3(&OBBDesc.vAlignAxis[0], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[0])));
	XMStoreFloat3(&OBBDesc.vAlignAxis[1], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[1])));
	XMStoreFloat3(&OBBDesc.vAlignAxis[2], XMVector3Normalize(XMLoadFloat3(&OBBDesc.vCenterAxis[2])));

	return OBBDesc;
}

CCollider * CCollider::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, TYPE eType)
{
	CCollider*		pInstance = new CCollider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(eType)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CCollider::Clone(void * pArg)
{
	CCollider*		pInstance = new CCollider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CCollider");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCollider::Free()
{
	__super::Free();

	Safe_Delete(m_pAABB_Original);
	Safe_Delete(m_pAABB);
	Safe_Delete(m_pOBB_Original);
	Safe_Delete(m_pOBB);
	Safe_Delete(m_pSphere_Original);
	Safe_Delete(m_pSphere);

#ifdef _DEBUG

	Safe_Release(m_pInputLayout);

	if (false == m_bClone)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
#endif // _DEBUG

}
