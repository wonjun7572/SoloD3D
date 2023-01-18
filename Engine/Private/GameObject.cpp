#include "..\Public\GameObject.h"
#include "GameInstance.h"
#include "GameUtils.h"

wstring CGameObject::m_pTransformComTag = TEXT("Com_Transform");

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	:m_pDevice(pDevice),
	m_pContext(pContext),
	m_bClone(false),
	m_bDead(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

CGameObject::CGameObject(const CGameObject & rhs)
	:m_pDevice(rhs.m_pDevice),
	m_pContext(rhs.m_pContext),
	m_bClone(true),
	m_bDead(false)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Init_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Init(void * pArg)
{
	GAMEOBJECTDESC GameObjectDesc;
	ZeroMemory(&GameObjectDesc, sizeof(GAMEOBJECTDESC));

	if (pArg != nullptr)
		GameObjectDesc = *(reinterpret_cast<GAMEOBJECTDESC*>(pArg));

	if (FAILED(Add_Component(CGameInstance::Get_StaticLevelIndex(), CGameInstance::m_pPrototypeTransformTag, m_pTransformComTag, reinterpret_cast<CComponent**>(&m_pTransformCom), &GameObjectDesc.TransformDesc)))
		return E_FAIL;

	return S_OK;
}

void CGameObject::Tick(_double TimeDelta)
{
}

void CGameObject::Late_Tick(_double TimeDelta)
{
}

HRESULT CGameObject::Render()
{
	return S_OK;
}

void CGameObject::Remove_Component(const wstring & strTag)
{
	auto iter = m_Components.find(strTag);

	if (iter == m_Components.end())
		return;

	Safe_Release(iter->second);
	iter = m_Components.erase(iter);
}

void CGameObject::Imgui_RenderComponentProperties()
{
	for (const auto& com : m_Components)
	{
		ImGui::Separator();
		char szName[128];
		CGameUtils::wc2c(com.first.c_str(), szName);

		ImGui::Text("%s", szName);
		com.second->Imgui_RenderProperty();
	}
}

HRESULT CGameObject::Add_Component(_uint iLevelIndex, const wstring& pPrototypeTag, const wstring& pComponentTag, CComponent ** ppOut, void * pArg)
{
	if (Find_Component(pPrototypeTag) != nullptr)
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CComponent*	pComponent = pGameInstance->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	m_Components.emplace(pComponentTag, pComponent);

	Safe_AddRef(pComponent);

	*ppOut = pComponent;

	Safe_Release(pGameInstance);

	return S_OK;
}

void CGameObject::Compute_CamDistance()
{
	_vector		vWorldPos = m_pTransformCom->Get_State(CTransform::STATE_TRANSLATION);

	CPipeLine*		pPipeLine = GET_INSTANCE(CPipeLine);

	_vector		vCamPos = XMLoadFloat4(&pPipeLine->Get_CamPosition());

	m_fCamDistance = XMVectorGetX(XMVector3Length(vWorldPos - vCamPos));

	RELEASE_INSTANCE(CPipeLine);
}

CComponent * CGameObject::Find_Component(const wstring& pComponentTag)
{
	auto iter = m_Components.find(pComponentTag);

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	Safe_Release(m_pTransformCom);

	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
	Safe_Delete_Array(m_szModelTag);
}

