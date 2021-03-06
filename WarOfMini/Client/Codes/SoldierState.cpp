#include "stdafx.h"
#include "SoldierState.h"
#include "State.h"

CSoldierState::CSoldierState(CPlayer* pSoldier)
	: m_pSoldier(pSoldier)
	, m_pInput(pSoldier->GetInput())
	, m_pAniIdx(pSoldier->Get_AniIdx())
	, m_pMoveDir(pSoldier->GetMoveDir())
{

}

CSoldierState::~CSoldierState()
{

}

int CSoldierState::InState()
{
	m_pkey = m_pSoldier->Get_KeyState();
	return 0;
}

int CSoldierState::OnState()
{
	m_pkey = m_pSoldier->Get_KeyState();
	return 0;
}

int CSoldierState::OutState()
{
	m_pkey = m_pSoldier->Get_KeyState();
	return 0;
}

void CSoldierState::ShootCheck(void)
{

}

void CSoldierState::Release(void)
{
	CState::Release();
}
