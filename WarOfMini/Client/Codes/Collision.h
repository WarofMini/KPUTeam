#pragma once

#include "Component.h"

class CCollision : public CComponent
{
public:
	CCollision();
	virtual ~CCollision();

	// CComponent��(��) ���� ��ӵ�
	virtual DWORD Release(void);
};

