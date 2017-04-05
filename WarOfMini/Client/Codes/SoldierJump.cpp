#include "stdafx.h"
#include "SoldierJump.h"
#include "Input.h"

CSoldierJump::CSoldierJump(CPlayer* pSoldier)
	: CSoldierState(pSoldier)
	, m_pSoldier(pSoldier)
{

}

CSoldierJump::~CSoldierJump()
{

}

int CSoldierJump::InState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);
	return 0;
}

int CSoldierJump::OnState()
{
	m_bShoot = m_pInput->Get_DIMouseState(CInput::DIM_LB);
	return 0;
}

int CSoldierJump::OutState()
{
	return 0;
}

void CSoldierJump::ShootCheck(void)
{
	
}

bool CSoldierJump::MoveKeyCheck(void)
{
	return true;
}

CSoldierJump* CSoldierJump::Create(CPlayer* pSoldier)
{
	return new CSoldierJump(pSoldier);
}

void CSoldierJump::Release(void)
{
	CSoldierState::Release();
}
