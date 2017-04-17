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

void CMeshMgr::CreateBoneNode(_uint uiObjNum, MATNODE* pMatNode)
{
	if (dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh))
		dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh)->CreateBoneNode(pMatNode);
}


void CMeshMgr::Add_ObbCheckList(_uint uiObjNum, const XMFLOAT4X4* pWorld, const CGameObject* pObj, MATNODE* pMatNode)
{
	dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh)->Add_ObbCheckList(pWorld, pObj, pMatNode);
}


XMFLOAT4X4 CMeshMgr::Get_TransMeshBone(_uint uiObjNum, _int iSubsetIdx, _int iIndex2, MATNODE* pMatNode)
{
	if (dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh))
		return dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh)->Get_TransBoneMatrix(iSubsetIdx, iIndex2, pMatNode);

	else
		return nullptr;
}


const XMFLOAT3* CMeshMgr::Get_MeshMin(_uint uiObjNum)
{
	return m_vecMeshData[uiObjNum]->pMesh->Get_Min();
}

const XMFLOAT3* CMeshMgr::Get_MeshMax(_uint uiObjNum)
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

void CMeshMgr::Render_MeshMgr(_uint uiObjNum, _uint uiTextureNumber, _bool bColliderDraw)
{
	if (m_vecMeshData[uiObjNum]->bBillboard)
		CGraphicDev::GetInstance()->SetCullEnable(FALSE);

	if (m_vecMeshData[uiObjNum]->bAlpha)
		CGraphicDev::GetInstance()->SetAlphaEnable(TRUE);

	m_vecMeshData[uiObjNum]->pMesh->Render(uiTextureNumber, bColliderDraw);

	if (m_vecMeshData[uiObjNum]->bBillboard)
		CGraphicDev::GetInstance()->SetCullEnable(TRUE);

	if (m_vecMeshData[uiObjNum]->bAlpha)
		CGraphicDev::GetInstance()->SetAlphaEnable(FALSE);
}

void CMeshMgr::RenderAnim_MeshMgr(_uint uiObjNum, CAnimationInfo* pAnimInfo, MATNODE* pMatNode, _uint uiTextureNumber, _ubyte byColor /*=0*/, _bool bColliderDraw /*= FALSE*/)
{
	if (dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh))
		dynamic_cast<CDynaicMesh*>(m_vecMeshData[uiObjNum]->pMesh)->RenderAnim(pAnimInfo, pMatNode, uiTextureNumber, byColor, bColliderDraw);
}

void CMeshMgr::RenderInst_MeshMgr(_uint uiObjNum, const vector<XMFLOAT4X4*>& vecObjWorld)
{
	if (m_vecMeshData[uiObjNum]->bBillboard)
		CGraphicDev::GetInstance()->SetCullEnable(FALSE);

	if (m_vecMeshData[uiObjNum]->bAlpha)
		CGraphicDev::GetInstance()->SetAlphaEnable(TRUE);

	m_vecMeshData[uiObjNum]->pMesh->RenderInst(vecObjWorld);

	if (m_vecMeshData[uiObjNum]->bBillboard)
		CGraphicDev::GetInstance()->SetCullEnable(TRUE);

	if (m_vecMeshData[uiObjNum]->bAlpha)
		CGraphicDev::GetInstance()->SetAlphaEnable(FALSE);
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

_uint CMeshMgr::Get_MeshVtxCnt(_uint uiObjNum)
{
	return m_vecMeshData[uiObjNum]->pMesh->GetVtxCnt();
}

VTXTEX* CMeshMgr::Get_MeshVtxTex(_uint uiObjNum)
{
	return m_vecMeshData[uiObjNum]->pMesh->GetVtxTex();
}

PxVec3* CMeshMgr::Get_MeshPxVtx(_uint uiObjNum)
{
	return m_vecMeshData[uiObjNum]->pMesh->GetPxVtx();
}

PxU32* CMeshMgr::Get_MeshPxIndex(_uint uiObjNum)
{
	return  m_vecMeshData[uiObjNum]->pMesh->GetPxIndex();
}
