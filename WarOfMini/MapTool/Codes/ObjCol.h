#ifndef ObjCol_h__
#define ObjCol_h__

class CObj;
class CMouseCol;
class CToolStaticObject;

class CObjCol
{
private:
	CObjCol();
	virtual ~CObjCol();

public:
	DECLARE_SINGLETON(CObjCol)

private:
	CMouseCol*		m_pMouseCol;
	float			m_fDist;
	bool			m_bModifyFinish;

	map<float, CObj*> m_mapObjMesh;

public:
	CToolStaticObject* m_pSelectMesh;

	CToolStaticObject* m_pCreateMesh;

public:
	HRESULT		Initialize(void);
	void		Update(void);
	void		Release(void);

public:
	void		CollisionObjMesh(void);

	void		ScaleControlMesh(CToolStaticObject* pMesh);

	void		ChangMeshInfo(CToolStaticObject* pMesh, int Number);
};

#endif