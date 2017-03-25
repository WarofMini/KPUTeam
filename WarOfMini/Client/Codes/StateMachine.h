#ifndef StateMachine_h__
#define StateMachine_h__

#include "Include.h"
#include "Component.h"
#include "State.h"

class CStateMachine : public CComponent
{
private:
	explicit	CStateMachine();
	virtual		~CStateMachine();

public:
	HRESULT				Add_State(CState* pState);
	void				Enter_State(DWORD	dwState);
	void				Update_State(DWORD dwState);
	CState::STATE		Get_Status(DWORD dwState);
	DWORD				Get_State();
	CState*				Get_State(DWORD dwState) { return m_vecState[dwState]; }

private:
	HRESULT				Ready_Machine(DWORD dwState);

private:
	DWORD				m_dwState;
	DWORD				m_dwStateCnt;
	vector<CState*>		m_vecState;

public:
	static	CStateMachine*	Create(DWORD dwStateCnt);

public:
	virtual	void	Release();
};

#endif // StateMachine_h__
