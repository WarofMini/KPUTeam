#ifndef ToolCollision_h__
#define ToolCollision_h__

#include "Component.h"

class CCollision : public CComponent
{
public:
	CCollision();
	virtual ~CCollision();

	// CComponent��(��) ���� ��ӵ�
	virtual DWORD Release(void);
};

#endif