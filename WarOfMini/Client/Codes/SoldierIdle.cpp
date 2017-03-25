#include "stdafx.h"
#include "SoldierIdle.h"
#include "Input.h"
#include "Player.h"

CSoldierIdle::CSoldierIdle(CPlayer* pSoldier)
	: CSoldierState(pSoldier)
	, m_pSoldier(pSoldier)
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
	if (CInput::GetInstance()->GetDIKeyStateOnce(DIK_UP))
	{
		m_pSoldier->PlayAnimation(PLAYER_RunForward, true);
		return 0;
	}

	return 1;
}

int CSoldierIdle::OutState()
{
	return 0;
}

CSoldierIdle* CSoldierIdle::Create(CPlayer* pSoldier)
{
	return new CSoldierIdle(pSoldier);
}

void CSoldierIdle::Release(void)
{
	CSoldierState::Release();
}
