#include "stdafx.h"
#include "Resources.h"

CResource::CResource(ID3D11Device * pGraphicDev, ID3D11DeviceContext * pContext)
: m_dwRefCount(new _ulong(0))
, m_pGraphicDev(pGraphicDev)
, m_pContext(pContext)
{
}

CResource::~CResource(void)
{
}

void CResource::Release(void)
{
	CComponent::Release();

	if ((*m_dwRefCount) == 0)
		Safe_Delete(m_dwRefCount);

	else
		--(*m_dwRefCount);

}
