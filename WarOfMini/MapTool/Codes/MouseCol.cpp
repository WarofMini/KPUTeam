#include "stdafx.h"
#include "MouseCol.h"
#include "ToolCamera.h"
#include "Mesh.h"
#include "CubeCol.h"

CMouseCol::CMouseCol(void)
:m_fTime(0.0f)
{
}

CMouseCol::~CMouseCol(void)
{
	Release();
}

POINT CMouseCol::GetMousePos(void)
{
	POINT	pt;

	GetCursorPos(&pt);
	ScreenToClient(g_hWnd, &pt);

	return pt;
}

CMouseCol * CMouseCol::Create(void)
{
	CMouseCol* pMouseCol = new CMouseCol;

	if(pMouseCol->Initialize())
		::Safe_Delete(pMouseCol);

	return pMouseCol;
}

HRESULT CMouseCol::Initialize(void)
{
	return S_OK;
}

void CMouseCol::Translation_ViewSpace(void)
{
	m_vCamEye = CToolCamera::GetInstance()->GetCamEye();

	D3DXMATRIX matProj = CToolCamera::GetInstance()->GetProj();

	D3DXVECTOR3		vTemp;

	vTemp.x = (float(g_vMouse.x) / (WINCX >> 1) - 1.f) / matProj._11;
	vTemp.y = (float(-g_vMouse.y) / (WINCY >> 1) + 1.f) / matProj._22;
	vTemp.z = 1.f;

	m_vPivotPos = D3DXVECTOR3(0.f, 0.f, 0.f);
	m_vRayDir = vTemp - m_vPivotPos;
	D3DXVec3Normalize(&m_vRayDir, &m_vRayDir);
}

void CMouseCol::Translation_Local(const D3DXMATRIX * pWorld)
{
	D3DXMATRIX matView = CToolCamera::GetInstance()->GetView();

	D3DXMatrixInverse(&matView, NULL, &matView);
	D3DXVec3TransformCoord(&m_vPivotPos, &m_vPivotPos, &matView);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &matView);

	D3DXMATRIX	matWorld;
	D3DXMatrixInverse(&matWorld, NULL, pWorld);
	D3DXVec3TransformCoord(&m_vPivotPos, &m_vPivotPos, &matWorld);
	D3DXVec3TransformNormal(&m_vRayDir, &m_vRayDir, &matWorld);

}

bool CMouseCol::PickObjMesh(CMesh * pMesh, D3DXMATRIX * pMatWorld, float* fDist, D3DXVECTOR3* vPickPos)
{
	if (pMesh == NULL)
		return false;


	if (pMesh->pVTXTex == NULL)
		return false;


	Translation_ViewSpace();

	Translation_Local(pMatWorld);

	BYTE* pbPositions = (BYTE*)pMesh->pVTXTex + pMesh->m_iVertexOffsets;


	int nOffset = 3;
	int nPrimitives = pMesh->m_iVertices / 3;


	D3DXVECTOR3 v0, v1, v2;

	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;

	for (int i = 0; i < nPrimitives; i++)
	{
		v0 = *(D3DXVECTOR3 *)(pbPositions + ((i * nOffset + 0) * pMesh->m_iVertexStrides));
		v1 = *(D3DXVECTOR3 *)(pbPositions + ((i * nOffset + 1) * pMesh->m_iVertexStrides));
		v2 = *(D3DXVECTOR3 *)(pbPositions + ((i * nOffset + 2) * pMesh->m_iVertexStrides));

		if (D3DXIntersectTri(&v0, &v1, &v2, &m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
		{
			if (fHitDistance < fNearHitDistance)
			{
				*fDist = fHitDistance;

				*vPickPos = v0 + (fuHitBaryCentric * (v1 - v0)) + (fvHitBaryCentric * (v2 - v0));

				D3DXVec3TransformCoord(vPickPos, vPickPos, pMatWorld);

				return true;
			}
		}
	}

	return false;
}

//Bounding Box 피킹
bool CMouseCol::PickBoundingBox(CVIBuffer* pBuffer, D3DXMATRIX * pMatWorld)
{
	if (pBuffer == NULL)
		return false;

	Translation_ViewSpace();
	Translation_Local(pMatWorld);

	VTXCOL*		m_pVtxCol = NULL;


	m_pVtxCol = (((CCubeCol*)pBuffer)->m_pVtxCol);


	float fuHitBaryCentric, fvHitBaryCentric, fHitDistance, fNearHitDistance = FLT_MAX;

	//0 (Z-) 앞면 위쪽 삼각형
	if (D3DXIntersectTri(&m_pVtxCol[1].vPos, &m_pVtxCol[0].vPos, &m_pVtxCol[2].vPos, 
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}

	//1 (Z-) 앞면 아래쪽 삼각형
	if (D3DXIntersectTri(&m_pVtxCol[3].vPos, &m_pVtxCol[2].vPos, &m_pVtxCol[0].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}

	//2 (Z+) 앞면 위쪽 삼각형
	if (D3DXIntersectTri(&m_pVtxCol[6].vPos, &m_pVtxCol[7].vPos, &m_pVtxCol[5].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}

	//3 (Z+)앞면 아래쪽 삼각
	if (D3DXIntersectTri(&m_pVtxCol[4].vPos, &m_pVtxCol[6].vPos, &m_pVtxCol[7].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}

	//4 (Y-)앞면 위쪽 삼각
	if (D3DXIntersectTri(&m_pVtxCol[2].vPos, &m_pVtxCol[3].vPos, &m_pVtxCol[6].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}

	//5 (Y-)앞면 아래쪽 삼각
	if (D3DXIntersectTri(&m_pVtxCol[7].vPos, &m_pVtxCol[6].vPos, &m_pVtxCol[3].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}

	//6 (Y+)앞면 위쪽 삼각
	if (D3DXIntersectTri(&m_pVtxCol[5].vPos, &m_pVtxCol[4].vPos, &m_pVtxCol[1].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}

	//7 (Y+)앞면 아래쪽 삼각
	if (D3DXIntersectTri(&m_pVtxCol[0].vPos, &m_pVtxCol[1].vPos, &m_pVtxCol[4].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}

	//8 (x-)앞면 위쪽 삼각
	if (D3DXIntersectTri(&m_pVtxCol[0].vPos, &m_pVtxCol[4].vPos, &m_pVtxCol[3].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}
	//9 (x-)앞면 아래쪽 삼각
	if (D3DXIntersectTri(&m_pVtxCol[7].vPos, &m_pVtxCol[3].vPos, &m_pVtxCol[4].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}


	//10 (x+)앞면 위쪽 삼각
	if (D3DXIntersectTri(&m_pVtxCol[5].vPos, &m_pVtxCol[1].vPos, &m_pVtxCol[6].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}

	//11 (x+)앞면 아래쪽 삼각
	if (D3DXIntersectTri(&m_pVtxCol[2].vPos, &m_pVtxCol[6].vPos, &m_pVtxCol[1].vPos,
		&m_vPivotPos, &m_vRayDir, &fuHitBaryCentric, &fvHitBaryCentric, &fHitDistance))
	{
		if (fHitDistance < fNearHitDistance)
		{
			fNearHitDistance = fHitDistance;

			return true;
		}
	}

	return false;
}


void CMouseCol::Release(void)
{
}
