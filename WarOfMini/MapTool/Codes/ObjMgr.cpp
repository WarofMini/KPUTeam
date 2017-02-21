#include "stdafx.h"
#include "ObjMgr.h"
#include "Obj.h"
#include "RenderMgr.h"
#include "StaticObject.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr()
{
}


CObjMgr::~CObjMgr()
{
}

HRESULT CObjMgr::SceneChange(void)
{
	map<wstring, list<CObj*>>::iterator iter = m_mapObj.begin();
	map<wstring, list<CObj*>>::iterator iter_end = m_mapObj.end();

	for (iter; iter != iter_end; ++iter)
	{
		list<CObj*>::iterator Listiter = iter->second.begin();
		list<CObj*>::iterator Listiter_end = iter->second.end();

		for (Listiter; Listiter != Listiter_end; )
		{
			if ((*Listiter)->GetReleaseType() == Release_Dynamic)
			{	
				::Safe_Delete(*Listiter);
				iter->second.erase(Listiter);
			}
			else
				++Listiter;
		}
	}

	return S_OK;
}

HRESULT CObjMgr::AddObject(wstring wstrKey, CObj * pObj)
{
	if (pObj == NULL)
	{
		MessageBox(NULL, L"System Message", wstrKey.c_str(), MB_OK);
		return E_FAIL;
	}

	map<wstring, list<CObj*>>::iterator	iter = m_mapObj.find(wstrKey);

	if (iter == m_mapObj.end())
	{
		m_mapObj[wstrKey] = list<CObj*>();
	}

	m_mapObj[wstrKey].push_back(pObj);


	return S_OK;
}

void CObjMgr::Update(void)
{
	map<wstring, list<CObj*>>::iterator iter = m_mapObj.begin();
	map<wstring, list<CObj*>>::iterator iter_end = m_mapObj.end();

	for (iter; iter != iter_end; ++iter)
	{
		list<CObj*>::iterator Listiter = iter->second.begin();
		list<CObj*>::iterator Listiter_end = iter->second.end();

		for (Listiter; Listiter != Listiter_end; )
		{
			(*Listiter)->Update();

			if ((*Listiter)->GetObjState() == STATE_DESTROY)
			{
				CRenderMgr::GetInstance()->DelRenderGroup((*Listiter)->GetRenderGroup(), (*Listiter));
				::Safe_Delete(*Listiter);
				Listiter = iter->second.erase(Listiter);
			}
			else
			{
				++Listiter;
			}
		}
	}

}
list<CObj*>*  CObjMgr::Get_ObjList(wstring wstrKey)
{
	map<wstring, list<CObj*>>::iterator iter = m_mapObj.find(wstrKey);

	if (iter == m_mapObj.end())
		return NULL;

	return &(*iter).second;
}

void CObjMgr::AllDestroy(void)
{
	map<wstring, list<CObj*>>::iterator iter = m_mapObj.begin();
	map<wstring, list<CObj*>>::iterator iter_end = m_mapObj.end();

	for (iter; iter != iter_end; ++iter)
	{
		list<CObj*>::iterator Listiter = iter->second.begin();
		list<CObj*>::iterator Listiter_end = iter->second.end();

		for (Listiter; Listiter != Listiter_end; ++Listiter)
		{
			if (((CToolStaticObject*)(*Listiter))->GetMode() == MODE_FIX)
				continue;

			(*Listiter)->SetObjState(STATE_DESTROY);
		}
	}

}
