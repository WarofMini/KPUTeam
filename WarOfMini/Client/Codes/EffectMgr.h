#ifndef EffectMgr_h__
#define EffectMgr_h__

#include "Include.h"


class CRcTex;
class CEffect;

class CEffectMgr
{
	DECLARE_SINGLETON(CEffectMgr)

private:
	CEffectMgr(void);
	~CEffectMgr(void);

public:
	void Release(void);
};


#endif