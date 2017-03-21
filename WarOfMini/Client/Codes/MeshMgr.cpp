#include "stdafx.h"
#include "MeshMgr.h"
#include "StaticMesh.h"
#include "DynamicMesh.h"
#include "Texture.h"
#include "GraphicDev.h"

IMPLEMENT_SINGLETON(CMeshMgr)

CMeshMgr::CMeshMgr(void)
{
}

CMeshMgr::~CMeshMgr(void)
{
}

void CMeshMgr::Add_ObbCheckList(_uint uiObjNum, const _matrix* pWorld, const CGameObject* pObj)
{
	dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh)->Add_ObbCheckList(pWorld, pObj);
}

_matrix CMeshMgr::Get_TransMeshBone(_uint uiObjNum, _int iSubsetIdx)
{
	_matrix matBone;

	D3DXMatrixIdentity(&matBone);

	if (dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh))
		return dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh)->Get_TransBoneMatrix(iSubsetIdx);
	else
		return matBone;
}

const _vec3* CMeshMgr::Get_MeshMin(_uint uiObjNum)
{
	return m_vecMeshData[uiObjNum]->pMesh->Get_Min();
}

const _vec3* CMeshMgr::Get_MeshMax(_uint uiObjNum)
{
	return m_vecMeshData[uiObjNum]->pMesh->Get_Max();
}

_bool CMeshMgr::Get_MeshUseAlpha(_uint uiObjNum)
{
	return m_vecMeshData[uiObjNum]->bAlpha;
}

void CMeshMgr::Ready_MeshMgr(MESHDATA* pMeshData)
{
	m_vecMeshData.push_back(pMeshData);
}

void CMeshMgr::Render_MeshMgr(_uint uiObjNum, _bool bColliderDraw)
{
	if (m_vecMeshData[uiObjNum]->bBillboard)
		CGraphicDev::GetInstance()->SetCullEnable(FALSE);

	m_vecMeshData[uiObjNum]->pMesh->Render(bColliderDraw);

	if (m_vecMeshData[uiObjNum]->bBillboard)
		CGraphicDev::GetInstance()->SetCullEnable(TRUE);
}

void CMeshMgr::RenderAnim_MeshMgr(_uint uiObjNum, CAnimationInfo* pAnimInfo, _bool bColliderDraw /*= FALSE*/)
{
	if (dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh))
		dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh)->RenderAnim(pAnimInfo, bColliderDraw);
}

void CMeshMgr::RenderInst_MeshMgr(_uint uiObjNum, const vector<_matrix*>& vecObjWorld)
{
	if (m_vecMeshData[uiObjNum]->bBillboard)
		CGraphicDev::GetInstance()->SetCullEnable(FALSE);

	m_vecMeshData[uiObjNum]->pMesh->RenderInst(vecObjWorld);

	if (m_vecMeshData[uiObjNum]->bBillboard)
		CGraphicDev::GetInstance()->SetCullEnable(TRUE);
}

void CMeshMgr::Release(void)
{
	size_t dwSize = m_vecMeshData.size();

	for (size_t i = 0; i < dwSize; ++i)
	{
		Safe_Release(m_vecMeshData[i]->pMesh);
		Safe_Delete(m_vecMeshData[i]);
	}

	m_vecMeshData.clear();

	delete this;
}