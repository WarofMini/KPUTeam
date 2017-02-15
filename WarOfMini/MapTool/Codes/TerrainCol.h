#ifndef ToolTerrainCol_h__
#define ToolTerrainCol_h__

#include "Collision.h"

class CTerrainCol :
	public CCollision
{
public:
	CTerrainCol();
	virtual ~CTerrainCol();

public:
	static CTerrainCol* Create(void);

public:
	void	CollisionTerrain(D3DXVECTOR3* pPos, VTXTEX* pVertex);
};

#endif