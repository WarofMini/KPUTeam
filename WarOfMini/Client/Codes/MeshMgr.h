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
	void CreateBoneNode(_uint uiObjNum, MATNODE* pMatNode);
	void Add_ObbCheckList(_uint uiObjNum, const XMFLOAT4X4* pWorld, const CGameObject* pObj, MATNODE* pMatNode);
	XMFLOAT4X4 Get_TransMeshBone(_uint uiObjNum, _int iSubsetIdx, _int iIndex2, MATNODE* pMatNode);
	const XMFLOAT3* Get_MeshMin(_uint uiObjNum);
	const XMFLOAT3* Get_MeshMax(_uint uiObjNum);
	_bool			Get_MeshUseAlpha(_uint uiObjNum);

public:
	void Ready_MeshMgr(MESHDATA* pMeshData);
	void Render_MeshMgr(_uint uiObjNum, _uint uiTextureNumber, _bool bColliderDraw = FALSE);
	void RenderAnim_MeshMgr(_uint uiObjNum, CAnimationInfo* pAnimInfo, MATNODE* pMatNode, _uint uiTextureNumber, _ubyte byColor = 0, _bool bColliderDraw = FALSE);
	void RenderInst_MeshMgr(_uint uiObjNum, const vector<XMFLOAT4X4*>& vecObjWorld);
	void Release(void);

public:
	_uint	Get_MeshVtxCnt(_uint uiObjNum);
	VTXTEX*	Get_MeshVtxTex(_uint uiObjNum);

private:
	vector<MESHDATA*>	m_vecMeshData;
};

#endif // MeshMgr_h__