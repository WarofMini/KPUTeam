#include "stdafx.h"
#include "Mesh.h"
#include "Texture.h"

CMesh::CMesh(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: CResource(pGraphicDev, pContext)
, m_pTexture(NULL)
, m_pVB(NULL)
, m_pIB(NULL)
, m_uiVtxCnt(0)
, m_uiIdxCnt(0)
, m_vMin(XMFLOAT3(0.f, 0.f, 0.f))
, m_vMax(XMFLOAT3(0.f, 0.f, 0.f))
, m_pBBoxVB(NULL)
, m_pBBoxIB(NULL)
, m_pVtxTex(NULL)
{
}

CMesh::~CMesh(void)
{
}

void CMesh::Reserve_ChildSize(const _ushort& wSize)
{
	m_vecChild.reserve(wSize);
}

void CMesh::Add_Child(CMesh* pChild)
{
	m_vecChild.push_back(pChild);
}

void CMesh::Clear_NullChild(void)
{
	_uint uiRealSize = m_vecChild.size();

	vector<CMesh*> vecStoreChile;
	vecStoreChile.reserve(uiRealSize);

	for (_uint uiSize = 0; uiSize < m_vecChild.size(); ++uiSize)
	{
		m_vecChild[uiSize]->Clear_NullChild();

		if (m_vecChild[uiSize]->Check_Remove())
		{
			Safe_Release(m_vecChild[uiSize]);
			--uiRealSize;
		}
	}

	vecStoreChile = m_vecChild;

	m_vecChild.clear();
	m_vecChild.reserve(uiRealSize);

	for (UINT uiSize = 0; uiSize < vecStoreChile.size(); ++uiSize)
	{
		if (vecStoreChile[uiSize])
			m_vecChild.push_back(vecStoreChile[uiSize]);
	}

	vecStoreChile.clear();
}
_bool CMesh::Check_Remove(void)
{
	if (m_vecChild.size()) return FALSE;
	if (m_uiVtxCnt) return FALSE;

	return TRUE;
}

void CMesh::Set_RootMinMax(void)
{
	XMFLOAT3 vTempMin, vTempMax;

	for (_uint uiSize = 0; uiSize < m_vecChild.size(); ++uiSize)
	{
		vTempMin = *m_vecChild[uiSize]->Get_Min();
		vTempMax = *m_vecChild[uiSize]->Get_Max();

		if (vTempMin.x < m_vMin.x)	m_vMin.x = vTempMin.x;
		if (vTempMin.y < m_vMin.y)	m_vMin.y = vTempMin.y;
		if (vTempMin.z < m_vMin.z)	m_vMin.z = vTempMin.z;
		if (vTempMax.x > m_vMax.x)	m_vMax.x = vTempMax.x;
		if (vTempMax.y > m_vMax.y)	m_vMax.y = vTempMax.y;
		if (vTempMax.z > m_vMax.z)	m_vMax.z = vTempMax.z;
	}
}

const XMFLOAT3* CMesh::Get_Min(void)
{
	return &m_vMin;
}

const XMFLOAT3* CMesh::Get_Max(void)
{
	return &m_vMax;
}

void CMesh::Release(void)
{
	CResource::Release();

	Safe_Delete_Array(m_pVtxTex);

	if (m_dwRefCount == NULL)
	{
		Safe_Release(m_pTexture);
		Safe_Release(m_pVB);
		Safe_Release(m_pIB);
		Safe_Release(m_pBBoxVB);
		Safe_Release(m_pBBoxIB);

		for (_uint uiSize = 0; uiSize < m_vecChild.size(); ++uiSize)
			m_vecChild[uiSize]->Release();

		m_vecChild.clear();
	}
}

VTXTEX* CMesh::GetVtxTex(void)
{
	return m_vecChild[0]->m_pVtxTex;
}

_uint CMesh::GetVtxCnt(void)
{
	return m_vecChild[0]->m_uiVtxCnt;
}

CTextures* CMesh::GetTexture(void)
{
	return m_pTexture;
}
