#include "stdafx.h"
#include "OtherSoldierState.h"
#include "State.h"

COtherSoldierState::COtherSoldierState(COtherPlayer* pSoldier)
	: m_pSoldier(pSoldier)
	, m_pAniIdx(pSoldier->Get_AniIdx())
	, m_pMoveDir(pSoldier->GetMoveDir())
{

}

COtherSoldierState::~COtherSoldierState()
{

}

int COtherSoldierState::InState()
{
	m_pkey = m_pSoldier->Get_KeyState();
	return 0;
}

int COtherSoldierState::OnState()
{
	m_pkey = m_pSoldier->Get_KeyState();
	return 0;
}

int COtherSoldierState::OutState()
{
	m_pkey = m_pSoldier->Get_KeyState();
	return 0;
}

void COtherSoldierState::ShootCheck(void)
{

}

void COtherSoldierState::Release(void)
{
	CState::Release();
}
