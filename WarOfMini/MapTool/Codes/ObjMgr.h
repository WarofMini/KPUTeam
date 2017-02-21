#ifndef ToolObjMgr_h__
#define ToolObjMgr_h__


class CObj;

class CObjMgr
{
private:
	CObjMgr();
	virtual ~CObjMgr();

public:
	DECLARE_SINGLETON(CObjMgr)

public:
	map<wstring, list<CObj*>> m_mapObj;

public:
	HRESULT SceneChange(void);
	HRESULT AddObject(wstring wstrKey, CObj* pObj);
	void	Update(void);
	list<CObj*>*  Get_ObjList(wstring wstrKey);

	void	AllDestroy(void);

};

#endif