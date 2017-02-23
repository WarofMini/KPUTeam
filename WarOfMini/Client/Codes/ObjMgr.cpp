#include "stdafx.h"
#include "ObjMgr.h"
#include "Object.h"
#include "COtherSoldier.h"

IMPLEMENT_SINGLETON(CObjMgr)

CObjMgr::CObjMgr()
{
}


CObjMgr::~CObjMgr()
{
}

HRESULT CObjMgr::SceneChange(void)
{
	map<wstring, list<CObject*>>::iterator iter = m_mapObj.begin();
	map<wstring, list<CObject*>>::iterator iter_end = m_mapObj.end();

	for (iter; iter != iter_end; ++iter)
	{
		list<CObject*>::iterator Listiter = iter->second.begin();
		list<CObject*>::iterator Listiter_end = iter->second.end();

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

HRESULT CObjMgr::AddObject(wstring wstrKey, CObject * pObj)
{
	if (pObj == NULL)
	{
		MessageBox(NULL, L"System Message", wstrKey.c_str(), MB_OK);
		return E_FAIL;
	}

	map<wstring, list<CObject*>>::iterator	iter = m_mapObj.find(wstrKey);

	if (iter == m_mapObj.end())
	{
		m_mapObj[wstrKey] = list<CObject*>();
	}

	m_mapObj[wstrKey].push_back(pObj);


	return S_OK;
}

void CObjMgr::Update(void)
{
	map<wstring, list<CObject*>>::iterator iter = m_mapObj.begin();
	map<wstring, list<CObject*>>::iterator iter_end = m_mapObj.end();

	for (iter; iter != iter_end; ++iter)
	{
		list<CObject*>::iterator Listiter = iter->second.begin();
		list<CObject*>::iterator Listiter_end = iter->second.end();

		for (Listiter; Listiter != Listiter_end; )
		{
			if ((*Listiter)->Update() == 100)
			{
				::Safe_Delete(*Listiter);
				iter->second.erase(Listiter);
			}
			else
				++Listiter;
		}
	}

}
list<CObject*>*  CObjMgr::Get_ObjList(wstring wstrKey)
{
	map<wstring, list<CObject*>>::iterator iter = m_mapObj.find(wstrKey);

	if (iter == m_mapObj.end())
		return NULL;

	return &(*iter).second;
}

Ser_PLAYER_DATA * CObjMgr::Get_Server_Data(int uId)
{
	map<wstring, list<CObject*>>::iterator iter = m_mapObj.find(L"OtherPlayer");

	if (iter == m_mapObj.end())
	{
		return NULL;
	}
	for(auto ptr : iter->second)
	{
		 int iCompare = ((COtherSoldier*)ptr)->GetPacketData()->ID;

		 if (iCompare == uId)
		 {
			 return ((COtherSoldier*)ptr)->GetPacketData();
		 }
	}

	return NULL;
}
