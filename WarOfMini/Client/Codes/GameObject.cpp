#include "stdafx.h"
#include "GameObject.h"
#include "Component.h"

CGameObject::CGameObject(ID3D11DeviceContext * pContext)
: m_pContext(pContext)
, m_pServer_PlayerData(NULL)
, m_iTextureNumber(0)
, m_pTransform(NULL)
, m_uiObjNum(0)
{
}

CGameObject::~CGameObject(void)
{
}

HRESULT CGameObject::Ready_Component(void)
{
	return S_OK;
}

const CComponent * CGameObject::Get_Component(const _tchar * pComponentTag)
{
	MAPCOMPONENT::iterator iter = find_if(m_mapComponent.begin(), m_mapComponent.end(), CTag_Finder(pComponentTag));

	if (iter == m_mapComponent.end())
		return NULL;

	return iter->second;
}

HRESULT CGameObject::Initialize(void)
{
	return S_OK;
}

_int CGameObject::Update(const _float& fTimeDelta)
{
	MAPCOMPONENT::iterator	iter = m_mapComponent.begin();
	MAPCOMPONENT::iterator	iter_end = m_mapComponent.end();

	for (; iter != iter_end; ++iter)
	{
		iter->second->Update(fTimeDelta);
	}

	return 0;
}

void CGameObject::Render(void)
{
}

void CGameObject::Release(void)
{
	for_each(m_mapComponent.begin(), m_mapComponent.end(), CRelease_Pair());
	m_mapComponent.clear();

	Safe_Delete(m_pServer_PlayerData);
}

void CGameObject::Set_TextureNumber(_uint uiTextureNum)
{
	m_iTextureNumber = uiTextureNum;
}

_uint CGameObject::Get_TextureNumber(void)
{
	return m_iTextureNumber;
}
