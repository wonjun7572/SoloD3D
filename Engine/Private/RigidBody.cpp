#include "..\Public\RigidBody.h"

CRigidBody::CRigidBody(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:CComponent(pDevice, pContext)
{
}

CRigidBody::CRigidBody(const CRigidBody & rhs)
	: CComponent(rhs)
{
}

HRESULT CRigidBody::Init_Prototype()
{
	return S_OK;
}

HRESULT CRigidBody::Init(void * pArg)
{
	return S_OK;
}

void CRigidBody::Tick(double dTime, double dTimeDelta)
{

}

CRigidBody * CRigidBody::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CRigidBody*		pInstance = new CRigidBody(pDevice, pContext);
	if (FAILED(pInstance->Init_Prototype()))
	{
		MSG_BOX("Failed TO Create : RigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

CComponent * CRigidBody::Clone(void * pArg)
{
	CRigidBody*		pInstance = new CRigidBody(*this);
	if (FAILED(pInstance->Init(pArg)))
	{
		MSG_BOX("Failed TO Clone : RigidBody");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CRigidBody::Free()
{
	__super::Free();

}
