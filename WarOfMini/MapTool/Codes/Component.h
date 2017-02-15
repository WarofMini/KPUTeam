#ifndef ToolComponent_h__
#define ToolComponent_h__


class CComponent
{
public:
	CComponent();
	virtual ~CComponent();

public:
	virtual	int  Update(void);
	virtual	DWORD Release(void) PURE;
};

#endif