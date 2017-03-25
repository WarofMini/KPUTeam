#include "stdafx.h"
#include "StateMachine.h"

CStateMachine::CStateMachine()
	: m_dwStateCnt(0)
	, m_dwState(0)
{

}

CStateMachine::~CStateMachine()
{

}

HRESULT CStateMachine::Ready_Machine(DWORD dwState)
{
	if (!dwState)
		return E_FAIL;

	m_vecState.reserve(dwState);
	m_dwStateCnt = dwState;
	return S_OK;
}

HRESULT CStateMachine::Add_State(CState* pState)
{
	if (pState == NULL)
		return E_FAIL;

	m_vecState.push_back(pState);

	return S_OK;
}

void CStateMachine::Enter_State(DWORD dwState)
{
	if (dwState >= m_dwStateCnt)
		return;

	m_dwState = dwState;
	m_vecState[dwState]->Begin_State();
}

void CStateMachine::Update_State(DWORD dwState)
{
	if (dwState >= m_dwStateCnt)
		m_vecState[m_dwState]->Update_State();
	else
		m_vecState[dwState]->Update_State();
}

DWORD CStateMachine::Get_State()
{
	return m_dwState;
}

CState::STATE CStateMachine::Get_Status(DWORD dwState)
{
	if (dwState >= m_dwStateCnt)
		return CState::STATE_END;

	return m_vecState[dwState]->Get_State();
}

CStateMachine* CStateMachine::Create(DWORD dwStateCnt)
{
	CStateMachine*	pInstance = new CStateMachine();
	if (FAILED(pInstance->Ready_Machine(dwStateCnt)))
		Safe_Release(pInstance);

	return pInstance;
}

void CStateMachine::Release()
{
	CComponent::Release();

	delete this;
}
