#include "stdafx.h"
#include "SoldierAttack.h"

CSoldierAttack::CSoldierAttack(CSoldier* pSoldier)
	: CSoldierState(pSoldier)
{

}

CSoldierAttack::~CSoldierAttack()
{

}

int CSoldierAttack::InState()
{
	return 0;
}

int CSoldierAttack::OnState()
{
	return 0;
}

int CSoldierAttack::OutState()
{
	return 0;
}

CSoldierAttack* CSoldierAttack::Create(CSoldier* pSoldier)
{
	return new CSoldierAttack(pSoldier);
}

void CSoldierAttack::Release(void)
{
	CSoldierState::Release();
}
