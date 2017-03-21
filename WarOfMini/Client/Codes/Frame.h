#ifndef Frame_h__
#define Frame_h__

#include "Include.h"

class CFrame
{
private:
	explicit CFrame(void);
			~CFrame(void);

public:
	_bool isPermit_Call(const _float& fTimeDelta);

public:
	HRESULT Ready_Frame(const _float& fCallLimit);

private:
	_float	m_fCallLimit;
	_float  m_fAccTimeDelta;


public:
	static  CFrame* Create(const _float& fCallLimit);
	void	Release(void);

};


#endif
