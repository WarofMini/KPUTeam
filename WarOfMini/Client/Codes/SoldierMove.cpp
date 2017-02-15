#include "stdafx.h"
#include "SoldierMove.h"

CSoldierMove::CSoldierMove(CSoldier* pSoldier)
	: CSoldierState(pSoldier)
{

}

CSoldierMove::~CSoldierMove()
{

}

int CSoldierMove::InState()
{
	return 0;
}

int CSoldierMove::OnState()
{
	return 0;
}

int CSoldierMove::OutState()
{
	return 0;
}

CSoldierMove* CSoldierMove::Create(CSoldier* pSoldier)
{
	return new CSoldierMove(pSoldier);
}

void CSoldierMove::Release(void)
{
	CSoldierState::Release();
}
