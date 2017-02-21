#include "stdafx.h"
#include "ObjCol.h"
#include "ObjMgr.h"
#include "Obj.h"
#include "StaticObject.h"
#include "Info.h"
#include "Input.h"
#include "MouseCol.h"
#include "Mesh.h"
#include "TimeMgr.h"
#include "ToolCamera.h"


IMPLEMENT_SINGLETON(CObjCol)

CObjCol::CObjCol()
{
	m_pMouseCol = NULL;
	m_fDist = 0.0f;
	m_pSelectMesh = NULL;
	m_pCreateMesh = NULL;
	m_bModifyFinish = false;
}

CObjCol::~CObjCol()
{
	Release();
}

HRESULT CObjCol::Initialize(void)
{
	m_pMouseCol = CMouseCol::Create();

	return S_OK;
}

void CObjCol::Update(void)
{
	//메시 충돌 체크
	CollisionObjMesh();


	if (m_pCreateMesh != NULL) //스태틱 메시 생성된 상태
	{
		m_pSelectMesh = NULL;

		ChangMeshInfo(m_pCreateMesh, 0);

		//=======================================================================
	}
	else //메시를 생성한 상태가 아닌경우
	{
		if (m_pSelectMesh != NULL) //피킹된 메시가 존재할 경우
		{

			ChangMeshInfo(m_pSelectMesh, 1);
		}
	}

}

void CObjCol::Release(void)
{
	::Safe_Delete(m_pMouseCol);
}

void CObjCol::CollisionObjMesh(void)
{
	if (g_vMouse.x <= 0.0f)
		return;
	
	list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"StaticObject")->begin();
	list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"StaticObject")->end();

	for (iter; iter != iter_end; iter++)
	{

		if((((CToolStaticObject*)(*iter))->GetMode() == MODE_CREATE) && m_pCreateMesh == NULL)
			m_pCreateMesh = ((CToolStaticObject*)(*iter));


		if ((((CToolStaticObject*)(*iter))->GetMode() != MODE_STOP) &&  
			(((CToolStaticObject*)(*iter))->GetMode() != MODE_FIX))
			continue ;

		if (m_pMouseCol->PickObjMesh(((CMesh*)((CToolStaticObject*)(*iter))->GetBuffer()),
			&((CToolStaticObject*)(*iter))->GetInfo()->m_matWorld, &m_fDist,
			((CToolStaticObject*)(*iter))->GetPickMeshPos()))
		{
			m_mapObjMesh.insert(map<float, CObj*>::value_type(m_fDist, (*iter)));

			m_fDist = 0.f;
		}
	}

	if (m_mapObjMesh.size() != 0)
	{
		CToolStaticObject* m_pMesh = (CToolStaticObject*)(m_mapObjMesh.begin()->second);

		//FIX모드가 아니라면
		if (m_pMesh->GetMode() == MODE_STOP && m_pCreateMesh == NULL)
		{
			if (g_bRBtnCheck == true && m_pSelectMesh == NULL)
			{
				m_pSelectMesh = (CToolStaticObject*)(m_mapObjMesh.begin()->second);
				m_pSelectMesh->SetMode(MODE_CREATE);
			}
		}

		g_vViewMouse = *m_pMesh->GetPickMeshPos();

		m_mapObjMesh.clear();

	}
	else
	{
		if (m_pSelectMesh != NULL)
		{
			if (g_bRBtnCheck == true)
				m_pSelectMesh = NULL;
		}
	}
}

void CObjCol::ScaleControlMesh(CToolStaticObject * pMesh)
{

	long dwMouseWheel = 0;

	//마우스 휠에 따른 스케일값 조정
	if (dwMouseWheel = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_Z))
	{
		if (dwMouseWheel > 0) //크기증가
		{
			D3DXVECTOR3 vScale;
			pMesh->GetInfo()->GetScale(&vScale);

			vScale.x += 0.01f;
			vScale.y += 0.01f;
			vScale.z += 0.01f;

			pMesh->GetInfo()->SetScale(&vScale);

		}
		else //크기 감소
		{
			D3DXVECTOR3 vScale;
			pMesh->GetInfo()->GetScale(&vScale);

			vScale.x -= 0.01f;
			vScale.y -= 0.01f;
			vScale.z -= 0.01f;

			pMesh->GetInfo()->SetScale(&vScale);
		}
	}
}

void CObjCol::ChangMeshInfo(CToolStaticObject * pMesh, int Number)
{
	//생성 모드
	if (pMesh->GetMode() == MODE_CREATE)
	{
		pMesh->GetInfo()->m_vPos = g_vViewMouse;

		//크기조절
		ScaleControlMesh(pMesh);

		if (CInput::GetInstance()->GetDIMouseState(CInput::DIM_LBUTTON))
		{
			m_bModifyFinish = true;
			pMesh->SetMode(MODE_MODIFY);
		}
	}
	//===================================================================
	//수정 모드
	if (pMesh->GetMode() == MODE_MODIFY)
	{
		//크기조절
		ScaleControlMesh(pMesh);

		//회전
		if (CInput::GetInstance()->GetDIMouseState(CInput::DIM_MBUTTON))
		{
			long dwMouseMove = 0;

			if (dwMouseMove = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_X))
			{
				//Y축 기준 회전
				pMesh->GetInfo()->m_fAngle[ANGLE_Y] += (float)(dwMouseMove * D3DXToRadian(90.f)) * CTimeMgr::GetInstance()->GetTime();
			}
		}

		if (g_vMouse.x > 15.0f)
		{
			if ((GetAsyncKeyState(VK_LBUTTON) & 1) && m_bModifyFinish == false)
			{
				pMesh->SetMode(MODE_STOP);

				if (Number == 0)
					m_pCreateMesh = NULL;
				else
					m_pSelectMesh = NULL;
			}
		}

		if (m_bModifyFinish == true)
		{
			m_bModifyFinish = false;
		}
	}
}
