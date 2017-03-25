#include "stdafx.h"
#include "SoldierState.h"
#include "State.h"

CSoldierState::CSoldierState(CPlayer* pSoldier)
	: m_pSoldier(pSoldier)
	, m_pInput(pSoldier->GetInput())
	, m_pAniIdx(pSoldier->Get_AniIdx())
{

}

CSoldierState::~CSoldierState()
{

}

int CSoldierState::InState()
{
	return 0;
}

int CSoldierState::OnState()
{
	return 0;
}

int CSoldierState::OutState()
{
	return 0;
}

void CSoldierState::ShootCheck(void)
{

}

void CSoldierState::Release(void)
{
	CState::Release();
}
