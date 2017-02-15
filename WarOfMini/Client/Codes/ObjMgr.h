#pragma once

#include "Include.h"

class CObject;
class CObjMgr
{
private:
	CObjMgr();
	virtual ~CObjMgr();

public:
	DECLARE_SINGLETON(CObjMgr)

public:
	map<wstring, list<CObject*>> m_mapObj;

public:
	HRESULT SceneChange(void);
	HRESULT AddObject(wstring wstrKey, CObject* pObj);
	void	Update(void);
	list<CObject*>*  Get_ObjList(wstring wstrKey);

};

