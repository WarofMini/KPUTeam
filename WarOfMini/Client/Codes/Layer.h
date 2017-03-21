#ifndef Layer_h__
#define Layer_h__

#include "Include.h"

class CGameObject;
class CComponent;

class CLayer
{
private:
	explicit CLayer(void);
	virtual ~CLayer(void);

private:
	typedef list<CGameObject*> OBJECTLIST;

	typedef map<const _tchar*, OBJECTLIST>  MAPOBJLIST;

	MAPOBJLIST			m_mapObjlist;

public:
	const CComponent* Get_Component(const _tchar* pObjectTag, const _tchar* pComponentTag, _ulong dwObjectIdx = 0);

public:
	HRESULT Ready_Object(const _tchar* pObjectTag, CGameObject* pGameObject);

public:
	_int Update(const _float& fTimeDelta);
	void Render(void);

public:
	static CLayer* Create(void);
	void			Release(void);

private:
	list<CGameObject*>* Find_ObjectList(const _tchar* pObjectTag);

};

#endif