#ifndef Component_h__
#define Component_h__

#include "Include.h"

class CComponent
{
public:
	explicit CComponent(void);
	virtual ~CComponent(void);

public:
	virtual	_int  Update(const _float& fTime);
	virtual	void  Release(void) PURE;
};

#endif
