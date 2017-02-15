#include "stdafx.h"
#include "SoldierState.h"
#include "State.h"

CSoldierState::CSoldierState(CSoldier* pSoldier)
	: m_pSoldier(pSoldier)
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

void CSoldierState::Release(void)
{
	CState::Release();
}
