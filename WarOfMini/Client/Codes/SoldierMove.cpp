#include "stdafx.h"
#include "SoldierMove.h"
#include "Input.h"

CSoldierMove::CSoldierMove(CPlayer* pSoldier)
	: CSoldierState(pSoldier)
	, m_pSoldier(pSoldier)
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
	if (CInput::GetInstance()->GetDIKeyStateLeave(DIK_UP))
	{
		m_pSoldier->PlayAnimation(PLAYER_idle, true);
		return 0;
	}

	return 1;
}

int CSoldierMove::OutState()
{
	return 0;
}

CSoldierMove* CSoldierMove::Create(CPlayer* pSoldier)
{
	return new CSoldierMove(pSoldier);
}

void CSoldierMove::Release(void)
{
	CSoldierState::Release();
}
