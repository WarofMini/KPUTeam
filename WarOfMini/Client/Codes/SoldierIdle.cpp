#include "stdafx.h"
#include "SoldierIdle.h"

CSoldierIdle::CSoldierIdle(CSoldier* pSoldier)
	: CSoldierState(pSoldier)
{

}

CSoldierIdle::~CSoldierIdle()
{

}

int CSoldierIdle::InState()
{
	return 0;
}

int CSoldierIdle::OnState()
{
	return 0;
}

int CSoldierIdle::OutState()
{
	return 0;
}

CSoldierIdle* CSoldierIdle::Create(CSoldier* pSoldier)
{
	return new CSoldierIdle(pSoldier);
}

void CSoldierIdle::Release(void)
{
	CSoldierState::Release();
}
