#include "stdafx.h"
#include "Layer.h"
#include "GameObject.h"
#include "Component.h"

CLayer::CLayer(void)
{
}

CLayer::~CLayer(void)
{
}

const CComponent * CLayer::Get_Component(const _tchar * pObjectTag, const _tchar * pComponentTag, _ulong dwObjectIdx)
{
	MAPOBJLIST::iterator	iter = find_if(m_mapObjlist.begin(), m_mapObjlist.end(), CTag_Finder(pObjectTag));

	if (iter == m_mapObjlist.end())
		return NULL;

	OBJECTLIST::iterator	iterlist = iter->second.begin();
	OBJECTLIST::iterator	iterlist_end = iter->second.end();

	for (_ulong dwIndex = 0; dwIndex < dwObjectIdx; ++dwIndex)
	{
		++iterlist;

		if (iterlist == iterlist_end)
			return NULL;
	}

	return (*iterlist)->Get_Component(pComponentTag);
}

HRESULT CLayer::Ready_Object(const _tchar * pObjectTag, CGameObject * pGameObject)
{
	list<CGameObject*>*	pObjectList = Find_ObjectList(pObjectTag);

	if (pObjectList == NULL)
	{
		list<CGameObject*> newObjectList;
		newObjectList.push_back(pGameObject);
		m_mapObjlist.insert(map<const _tchar*, list<CGameObject*>>::value_type(pObjectTag, newObjectList));
	}
	else
	{
		pObjectList->push_back(pGameObject);
	}

	return S_OK;
}

_int CLayer::Update(const _float & fTimeDelta)
{
	MAPOBJLIST::iterator iter = m_mapObjlist.begin();
	MAPOBJLIST::iterator iter_end = m_mapObjlist.end();

	_int iResult = 0;

	for (iter; iter != iter_end; ++iter)
	{
		OBJECTLIST::iterator iterList = iter->second.begin();
		OBJECTLIST::iterator iterList_end = iter->second.end();

		for (; iterList != iterList_end;)
		{
			iResult = (*iterList)->Update(fTimeDelta);

			if (iResult == 1)
			{
				Safe_Release(*iterList);
				iterList = iter->second.erase(iterList);
			}
			else
			{
				++iterList;
			}
		}
	}


	/*
	원래코드
	for (iter; iter != iter_end; ++iter)
	{
		OBJECTLIST::iterator iterList = iter->second.begin();
		OBJECTLIST::iterator iterList_end = iter->second.end();

		for (; iterList != iterList_end; ++iterList)
		{
			iResult = (*iterList)->Update(fTimeDelta);

			if (iResult & 0x80000000)
				return iResult;
		}
	}
	*/

	return iResult;
}

void CLayer::Render(void)
{
	MAPOBJLIST::iterator iter = m_mapObjlist.begin();
	MAPOBJLIST::iterator iter_end = m_mapObjlist.end();

	for (iter; iter != iter_end; ++iter)
	{
		OBJECTLIST::iterator iterList = iter->second.begin();
		OBJECTLIST::iterator iterList_end = iter->second.end();

		for (; iterList != iterList_end; ++iterList)
		{
			(*iterList)->Render();
		}
	}
}

CLayer * CLayer::Create(void)
{
	CLayer* pLayer = new CLayer;

	return pLayer;
}

void CLayer::Release(void)
{
	MAPOBJLIST::iterator	iter = m_mapObjlist.begin();
	MAPOBJLIST::iterator	iter_end = m_mapObjlist.end();

	for (; iter != iter_end; ++iter)
	{
		OBJECTLIST::iterator	iterList = iter->second.begin();
		OBJECTLIST::iterator	iterList_end = iter->second.end();

		for (; iterList != iterList_end; ++iterList)
			Safe_Release((*iterList));

		iter->second.clear();
	}

	m_mapObjlist.clear();

	delete this;
}

list<CGameObject*>* CLayer::Find_ObjectList(const _tchar * pObjectTag)
{
	map<const _tchar*, list<CGameObject*>>::iterator	iter = find_if(m_mapObjlist.begin(), m_mapObjlist.end(), CTag_Finder(pObjectTag));

	if (iter == m_mapObjlist.end())
		return NULL;

	return &iter->second;
}
