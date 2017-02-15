#include "stdafx.h"
#include "TerrainCol.h"


CTerrainCol::CTerrainCol()
{
}


CTerrainCol::~CTerrainCol()
{
}

CTerrainCol * CTerrainCol::Create(void)
{
	CTerrainCol* pCollision = new CTerrainCol;

	return pCollision;
}

void CTerrainCol::CollisionTerrain(D3DXVECTOR3 * pPos, VTXTEX * pVertex)
{
	int		iIndex = (int(pPos->z) / VERTEXINTERVAL) * VERTEXCOUNTX + (int(pPos->x) / VERTEXINTERVAL);

	float	fRatioX = (pPos->x - pVertex[iIndex + VERTEXCOUNTX].vPos.x) / VERTEXINTERVAL;
	float	fRatioZ = (pVertex[iIndex + VERTEXCOUNTX].vPos.z - pPos->z) / VERTEXINTERVAL;

	D3DXPLANE		Plane;


	if (fRatioX > fRatioZ)	 // ������ ��
	{
		D3DXPlaneFromPoints(&Plane, &pVertex[iIndex + VERTEXCOUNTX].vPos,
									&pVertex[iIndex + VERTEXCOUNTX + 1].vPos,
									&pVertex[iIndex + 1].vPos);
	}

	else					// ���� �Ʒ�
	{
		D3DXPlaneFromPoints(&Plane, &pVertex[iIndex + VERTEXCOUNTX].vPos,
			&pVertex[iIndex + 1].vPos,
			&pVertex[iIndex].vPos);
	}

	// y  = (-Ax - Cz - D) / B;

	pPos->y = (-Plane.a * pPos->x - Plane.c * pPos->z - Plane.d) / Plane.b;
	pPos->y += 1.f;
}
