#include "stdafx.h"
#include "State.h"

CState::CState()
: m_eState(STATE_IN)
{

}

CState::~CState()
{

}

void CState::Begin_State()
{
	m_eState = STATE_IN;
}

void CState::Update_State()
{
	switch(m_eState)
	{
	case STATE_IN :
		if(InState() == 0)
			m_eState = STATE_ON;
		break;

	case STATE_ON :
		if(OnState() == 0)
			m_eState = STATE_OUT;
		break;

	case STATE_OUT :
		if(OutState() == 0)
			m_eState = STATE_END;
		break;
	}
}

CState::STATE CState::Get_State()
{
	return m_eState;
}

int CState::InState()
{
	return 0;
}

int CState::OnState()
{
	return 0;
}

int CState::OutState()
{
	return 0;
}

void CState::Release()
{

}