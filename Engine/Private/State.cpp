#include "..\Public\State.h"

CState::CState(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
	:CComponent(pDevice, pDeviceContext)
{
}

CState::CState(const CState & rhs)
	:CComponent(rhs)
{
}

HRESULT CState::Initialize_Proto(void)
{
	if (FAILED(__super::Init_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CState::Initialize_Clone(void * pArg)
{
	if (FAILED(__super::Init(pArg)))
		return E_FAIL;

	return S_OK;
}

void CState::Tick(_double TimeDelta)
{
	auto CurState = find_if(m_mapState.begin(), m_mapState.end(), CTag_Finder(m_CurStateName));

	for (auto Changer : m_mapChanger[m_CurStateName])
	{
		if (true == Changer.Changer_Func())
		{
			m_NextStateName = Changer.szNextState;
			break;
		}
	}
	
	if (lstrcmp(m_NextStateName, TEXT("")))
	{
		if(nullptr != CurState->second.State_End)
			CurState->second.State_End(TimeDelta);

		m_BeforeStateName = m_CurStateName;
		m_CurStateName = m_NextStateName;

		if (nullptr != m_mapState[m_CurStateName].State_Start)
			m_mapState[m_CurStateName].State_Start(TimeDelta);

		m_NextStateName = TEXT("");
	}
	else
	{
		if (nullptr != CurState->second.State_Tick)
			CurState->second.State_Tick(TimeDelta);
	}
}

CState & CState::Set_Root(const _tchar * szStateName)
{
	m_RootStateName = szStateName;

	return *this;
}

CState & CState::Add_State(const _tchar * szStateName)
{
	STATE eState;
	ZeroMemory(&eState, sizeof(eState));

	m_mapState.emplace(szStateName, eState);
	m_CurStateName = szStateName;

	return *this;
}

CState * CState::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pDeviceContext)
{
	CState* pInstance = new CState(pDevice, pDeviceContext);

	if (FAILED(pInstance->Initialize_Proto()))
	{
		MSG_BOX("State Create Fail");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

CComponent * CState::Clone(void * pArg)
{
	CState* pInstance = new CState(*this);

	if (FAILED(pInstance->Initialize_Clone()))
	{
		MSG_BOX("State Clone Fail");
		Safe_Release(pInstance);
		return nullptr;
	}

	return pInstance;
}

void CState::Free(void)
{
	__super::Free();
}
