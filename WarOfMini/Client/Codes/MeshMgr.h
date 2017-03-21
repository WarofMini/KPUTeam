#ifndef MeshMgr_h__
#define MeshMgr_h__

#include "Include.h"

class CGameObject;
class CAnimationInfo;
class CTextures;

class CMeshMgr
{
	DECLARE_SINGLETON(CMeshMgr)

private:
	CMeshMgr(void);
	~CMeshMgr(void);

public:
	void Add_ObbCheckList(_uint uiObjNum, const _matrix* pWorld, const CGameObject* pObj);
	_matrix Get_TransMeshBone(_uint uiObjNum, _int iSubsetIdx);
	const _vec3* Get_MeshMin(_uint uiObjNum);
	const _vec3* Get_MeshMax(_uint uiObjNum);
	_bool Get_MeshUseAlpha(_uint uiObjNum);

public:
	void Ready_MeshMgr(MESHDATA* pMeshData);
	void Render_MeshMgr(_uint uiObjNum, _bool bColliderDraw = FALSE);
	void RenderAnim_MeshMgr(_uint uiObjNum, CAnimationInfo* pAnimInfo, _bool bColliderDraw = FALSE);
	void RenderInst_MeshMgr(_uint uiObjNum, const vector<_matrix*>& vecObjWorld);
	void Release(void);

private:
	vector<MESHDATA*>	m_vecMeshData;
};

#endif // MeshMgr_h__