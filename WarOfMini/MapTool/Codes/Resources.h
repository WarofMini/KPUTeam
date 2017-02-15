#ifndef ToolResources_h__
#define ToolResources_h__

#include "Component.h"

class CResources :
	public CComponent
{
public:
	CResources();
	explicit CResources(CResources& rhs);
	virtual ~CResources();

public:
	virtual CResources* CloneResource(void)PURE;
	virtual DWORD Release(void);
	void	AddRef(void);

protected:
	DWORD		m_dwRefCount;

};

#endif