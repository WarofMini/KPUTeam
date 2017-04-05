#include "stdafx.h"
#include "Calculator.h"
#include "Transform.h"
#include "CameraMgr.h"
#include "MeshMgr.h"
#include "Layer.h"
#include "Management.h"
#include "Scene.h"
#include "DefaultObject.h"

CCalculator::CCalculator(void)
	: m_pTransform(NULL)
	, m_iMeshSize(0)
	, m_bIsCol(true)
	, m_bJump(false)
	, m_fMagicNumber(1.f)
{
}

CCalculator::~CCalculator(void)
{

}

_bool CCalculator::Get_IsCol(void)
{
	return m_bIsCol;
}

void CCalculator::Set_IsCol(_bool bCol)
{
	m_bIsCol = bCol;
}

void CCalculator::Set_Jump(_bool bJump)
{
	m_bJump = bJump;
}

const XMFLOAT3 CCalculator::GetCollPos(void)
{
	return m_vCollPos;
}

void CCalculator::SetCollPos(const XMFLOAT3 vCollPos)
{
	m_vCollPos = vCollPos;
}

void CCalculator::Set_MagicNum(const _float& fMagic)
{
	m_fMagicNumber = fMagic;
}

void CCalculator::Set_Transform(CTransform* pTransform)
{
	if (pTransform == NULL)
		return;
	m_pTransform = pTransform;
}

_int CCalculator::Update(const _float& fTime)
{
	
	// 지형 충돌
	//if (m_pTransform == NULL)
	//	return 0;

	//_float tmin = 1000.0f;
	//_float fMagicNumber = 2.f;
	//XMVECTOR vRayDir = XMVectorSet(0.0f, -1.0f, 0.0f, 0.0f);
	//XMVECTOR vOrigin = XMLoadFloat3(&m_pTransform->m_vPos);

	//_float posy = m_pTransform->m_vPos.y + fMagicNumber;

	//XMFLOAT3 test = XMFLOAT3(m_pTransform->m_vPos.x, posy, m_pTransform->m_vPos.z);

	//vOrigin = XMLoadFloat3(&test);

	////반직선의 원점, 방향, 삼각형 정점0, 1, 2, 교점의 매개변수

	//
	//CLayer* pLayer = CManagement::GetInstance()->GetScene()->FindLayer(L"Layer_GameLogic");

	//if (pLayer == NULL)
	//	return 0;

	//list<CGameObject*>* pObjList = pLayer->Find_ObjectList(L"StaticObject");

	//if (pObjList == NULL)
	//	return 0;

	//list<CGameObject*>::iterator iter = pObjList->begin();
	//list<CGameObject*>::iterator iter_end = pObjList->end();


	//for (iter; iter != iter_end; ++iter) //배치된 오브젝트를 순휘
	//{

	//	int nOffset = 3;

	//	int nPrimitives = CMeshMgr::GetInstance()->Get_MeshVtxCnt( ( (CDefaultObj*)(*iter)  )->GetObjNum()) / 3;

	//	VTXTEX*	m_pTex = CMeshMgr::GetInstance()->Get_MeshVtxTex(((CDefaultObj*)(*iter))->GetObjNum());


	//	//XMMATRIX	matWorld;

	//	//XMVECTOR    vecTest;
	//	//matWorld = XMMatrixInverse(&vecTest, XMLoadFloat4x4(&((CTransform*)((CDefaultObj*)(*iter))->Get_Component(L"Com_Transform"))->m_matWorld));

	//	//vOrigin = XMVector3TransformCoord(vOrigin, matWorld);
	// //   vRayDir = XMVector3TransformNormal(vRayDir, matWorld);
	//	//vRayDir = XMVector3Normalize(vRayDir);



	//	if (m_pTex == NULL)
	//		continue;

	//	XMVECTOR v0, v1, v2;
	//	
	//	for (int i = 0; i < nPrimitives; ++i)
	//	{

	//		v0 = XMLoadFloat3(&m_pTex[i * nOffset + 0].vPos);
	//		v1 = XMLoadFloat3(&m_pTex[i * nOffset + 1].vPos);
	//		v2 = XMLoadFloat3(&m_pTex[i * nOffset + 2].vPos);

	//		//XMMATRIX  matWorld;

	//		//matWorld = XMLoadFloat4x4(&((CTransform*)((CDefaultObj*)(*iter))->Get_Component(L"Com_Transform"))->m_matWorld);

	//		//v0 = XMVector3TransformCoord(v0, matWorld);
	//		//v1 = XMVector3TransformCoord(v1, matWorld);
	//		//v2 = XMVector3TransformCoord(v2, matWorld);

	//		float fDist = 0.0f;

	//		if (XNA::IntersectRayTriangle(vOrigin, vRayDir, v0, v1, v2, &fDist))
	//		{
	//			if (fDist < tmin)
	//			{
	//				tmin = fDist;
	//			}
	//		}
	//	}
	//}

	//if (tmin != 1000.0f &&  tmin > fMagicNumber  &&  m_bJump)
	//	m_bJump = false; 

	//if (m_bJump)
	//{
	//	m_bIsCol = false;
	//	return 0;
	//}

	//if (tmin < fMagicNumber)
	//{
	//	XMVECTOR vPos = vOrigin + (vRayDir * tmin );

	//	XMStoreFloat3(&m_vCollPos, vPos);

	//	XMStoreFloat3(&m_pTransform->m_vPos, vPos);
	//	m_bIsCol = true;
	//}
	//else
	//	m_bIsCol = false;
		
	return 0;
}

void CCalculator::Release(void)
{
	CComponent::Release();

	delete this;
}

HRESULT CCalculator::Ready_Caculator(void)
{
	m_vRayDir = XMFLOAT3(0.f, -1.f, 0.f);
	return S_OK;
}

CCalculator* CCalculator::Create(void)
{
	CCalculator* pInstance = new CCalculator;

	if (FAILED(pInstance->Ready_Caculator()))
	{
		MSG_BOX(L"CCalculator Created Failed");
		Safe_Release(pInstance);
	}
	return pInstance;
}

void CCalculator::Translation_ViewSpace(POINT ptMouse)
{
	XMMATRIX matProj;

	matProj = XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraProj());

	XMFLOAT3 vTemp;

	vTemp.x = (float(ptMouse.x) / (WINCX >> 1) - 1.f) / matProj._11;
	vTemp.y = (float(-ptMouse.y) / (WINCY >> 1) + 1.f) / matProj._22;
	vTemp.z = 1.f;

	m_vPivotPos = XMFLOAT3(0.0f, 0.0f, 0.0f);


	XMVECTOR vRayDir;

	vRayDir = XMLoadFloat3(&vTemp) - XMLoadFloat3(&m_vPivotPos);

	vRayDir = XMVector3Normalize(vRayDir);

	XMStoreFloat3(&m_vRayDir, vRayDir);
}

void CCalculator::Translation_Local(const XMFLOAT4X4* pWorld)
{
	XMMATRIX matView;

	matView = XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView());


	XMVECTOR    vecTest = XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f);
	matView = XMMatrixInverse(&vecTest, matView);
 
	XMVECTOR vPivotPos, vRayDir;

	vPivotPos = XMLoadFloat3(&m_vPivotPos);
	vRayDir   = XMLoadFloat3(&m_vRayDir);

	vPivotPos = XMVector3TransformCoord(vPivotPos, matView);
	vRayDir = XMVector3TransformNormal(vRayDir, matView);

	XMMATRIX  matWorld;
	matWorld = XMMatrixInverse(&vecTest, XMLoadFloat4x4(pWorld));

	vPivotPos = XMVector3TransformCoord(vPivotPos, matWorld);
	vRayDir = XMVector3TransformNormal(vRayDir, matWorld);

	XMStoreFloat3(&m_vRayDir, vRayDir);
	XMStoreFloat3(&m_vPivotPos, vPivotPos);
}
