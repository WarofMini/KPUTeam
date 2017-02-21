#include "stdafx.h"
#include "StaticObject.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "Info.h"
#include "VIBuffer.h"
#include "ShaderMgr.h"
#include "ToolCamera.h"
#include "ObjMgr.h"
#include "Shader.h"
#include "Device.h"
#include "RenderMgr.h"
#include "ResourcesMgr.h"
#include "MouseCol.h"
#include "CubeCol.h"
#include "Input.h"
#include "TimeMgr.h"

CToolStaticObject::CToolStaticObject()
{

	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pMouseCol = NULL;
	m_pBoundingBox = NULL;

	ZeroMemory(&m_tInfo, sizeof(OBJ_INFO));

	m_eMode = MODE_CREATE;

	m_eRenderGroup = TYPE_NONEALPHA;
}

CToolStaticObject::~CToolStaticObject()
{
	Release();
}

HRESULT CToolStaticObject::Initialize(void)
{
	if (FAILED(AddComponent()))
		return E_FAIL;

	CRenderMgr::GetInstance()->AddRenderGroup(m_eRenderGroup, this);
	
	m_pMouseCol = CMouseCol::Create();

	return S_OK;
}

int CToolStaticObject::Update(void)
{
	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	CObj::Update();

	//if (m_eMode == MODE_FIX)
	//{
	//	m_pMouseCol->PickObjMesh((CMesh*)m_pBuffer, &m_pInfo->m_matWorld);
	//}
	//else
	//{
	//	SettingWork();
	//}

	//오브젝트 피킹체크
	//if(m_pMouseCol->PickObjMesh((CMesh*)m_pBuffer, &m_pInfo->m_matWorld))
	//{
	//}

	//바운딩 박스 피킹
	//if(m_pMouseCol->PickBoundingBox(m_pBoundingBox, &m_pInfo->m_matWorld))
	//{
	//}

	return 0;
}

void CToolStaticObject::Render(void)
{
	ConstantBuffer cb;
	D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
	D3DXMatrixTranspose(&cb.matView, &CToolCamera::GetInstance()->m_matView);
	D3DXMatrixTranspose(&cb.matProjection, &CToolCamera::GetInstance()->m_matProj);
	m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);

	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBuffer->m_ConstantBuffer);

	//////////////////
	m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

	m_pBuffer->Render();

	BoundingBoxRender();
}


CToolStaticObject * CToolStaticObject::Create(wstring strName)
{
	CToolStaticObject* pObj = new CToolStaticObject;

	pObj->SetStrName(strName);

	if (FAILED(pObj->Initialize()))
		::Safe_Delete(pObj);



	return pObj;
}

void CToolStaticObject::Release(void)
{
	//::Safe_Delete(m_pBuffer);
	//::Safe_Delete(m_pInfo);
	//::Safe_Delete(m_pTexture);
	::Safe_Delete(m_pMouseCol);
	::Safe_Delete(m_pBoundingBox);
}

HRESULT CToolStaticObject::AddComponent(void)
{
	CComponent* pComponent = NULL;

	//TransForm
	pComponent = m_pInfo = CInfo::Create(g_vLook);
	NULL_CHECK_RETURN(pComponent, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Transform", pComponent));


	//StaticMesh
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, m_strName.c_str());
	m_pBuffer = dynamic_cast<CStaticMesh*>(pComponent);
	NULL_CHECK_RETURN(m_pBuffer, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Mesh", pComponent));

	m_pBoundingBox = CCubeCol::Create(((CMesh*)m_pBuffer)->m_vMin, ((CMesh*)m_pBuffer)->m_vMax, D3DXCOLOR(1.f, 0.f, 0.f, 1.f));

	//Texture
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Couch");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");

	return S_OK;
}

void CToolStaticObject::SetStrName(wstring _strName)
{
	m_strName = _strName;
}

wstring CToolStaticObject::GetStrName(void)
{
	return m_strName;
}

void CToolStaticObject::BoundingBoxRender(void)
{
	ConstantBuffer cb2;
	D3DXMatrixTranspose(&cb2.matWorld, &m_pInfo->m_matWorld);
	D3DXMatrixTranspose(&cb2.matView, &CToolCamera::GetInstance()->m_matView);
	D3DXMatrixTranspose(&cb2.matProjection, &CToolCamera::GetInstance()->m_matProj);

	m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pBoundingBox->m_ConstantBuffer, 0, NULL, &cb2, 0, 0);

	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBoundingBox->m_ConstantBuffer);
	m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);

	m_pBoundingBox->Render();
}

void CToolStaticObject::SettingWork(void)
{
	/*
	//초기  생성하고 이동상태일 경우
	if (m_eMode == MODE_MOVE)
	{
		//g_vViewMouse : 마우스와 메시의 충돌 포지션
		m_pInfo->m_vPos = D3DXVECTOR3(g_vViewMouse);

		//R버튼을 누르면(우클릭)
		if (CInput::GetInstance()->GetDIMouseState(CInput::DIM_RBUTTON))
			m_eMode = MODE_ANGLE;
	}
	//회전, 크기 변환
	else if (m_eMode == MODE_ANGLE)
	{
		if (CToolCamera::GetInstance()->GetMouseFixCheck() == false)
		{
			CToolCamera::GetInstance()->SetMouseFixCheck(true);
		}

		ScaleAndRotateChange();

		if (CInput::GetInstance()->GetDIMouseState(CInput::DIM_LBUTTON))
		{
			m_eMode = MODE_STOP;
			CToolCamera::GetInstance()->SetMouseFixCheck(false);
		}

	}
	//STOP 상태
	else if (m_eMode == MODE_STOP)
	{
		if (g_vMouse.x > 0.0f)
		{

			//STOP 상황에서 메시 피킹이 된경우
			if (m_pMouseCol->PickObjMesh((CMesh*)m_pBuffer, &m_pInfo->m_matWorld))
			{
				if (CInput::GetInstance()->GetDIMouseState(CInput::DIM_LBUTTON))
				{

					list<CObj*>::iterator iter = CObjMgr::GetInstance()->Get_ObjList(L"StaticObject")->begin();
					list<CObj*>::iterator iter_end = CObjMgr::GetInstance()->Get_ObjList(L"StaticObject")->end();

					for (iter; iter != iter_end; iter++)
					{
						if (((CToolStaticObject*)(*iter))->GetMode() == MODE_MODIFY)
						{
							((CToolStaticObject*)(*iter))->SetMode(MODE_STOP);
						}
					}

					//수정 상태로 변경
					m_eMode = MODE_MODIFY;

				}
			}
		}
	}
	//수정상태
	else if (m_eMode == MODE_MODIFY)
	{
		ScaleAndRotateChange();
		//우클릭 시 STOP상태로 변환
		if (CInput::GetInstance()->GetDIMouseState(CInput::DIM_RBUTTON))
		{
			m_eMode = MODE_STOP;
		}
	}

	*/
}

void CToolStaticObject::SetMode(OBJECT_MODE eMode)
{
	m_eMode = eMode;
}

OBJECT_MODE CToolStaticObject::GetMode(void)
{
	return m_eMode;
}

void CToolStaticObject::ScaleAndRotateChange(void)
{

	long dwMouseMove = 0;
	long dwMouseWheel = 0;
	//회전
	if (dwMouseMove = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_X))
	{
		//Y축 기준 회전
		m_pInfo->m_fAngle[ANGLE_Y] += (float)(dwMouseMove * D3DXToRadian(90.f)) * CTimeMgr::GetInstance()->GetTime();
	}

	//마우스 휠에 따른 스케일값 조정
	if (dwMouseWheel = CInput::GetInstance()->GetDIMouseMove(CInput::DIM_Z))
	{
		if (dwMouseWheel > 0) //크기증가
		{
			D3DXVECTOR3 vScale;
			m_pInfo->GetScale(&vScale);

			vScale.x += 0.01f;
			vScale.y += 0.01f;
			vScale.z += 0.01f;

			m_pInfo->SetScale(&vScale);

		}
		else //크기 감소
		{
			D3DXVECTOR3 vScale;
			m_pInfo->GetScale(&vScale);

			vScale.x -= 0.01f;
			vScale.y -= 0.01f;
			vScale.z -= 0.01f;

			m_pInfo->SetScale(&vScale);
		}
	}
}

CVIBuffer * CToolStaticObject::GetBuffer(void)
{
	return m_pBuffer;
}

D3DXVECTOR3 * CToolStaticObject::GetPickMeshPos(void)
{
	return &m_vPickMeshPos;
}

OBJ_INFO * CToolStaticObject::GetObjInfo(void)
{
	return &m_tInfo;
}

void CToolStaticObject::SetObjInfo(OBJ_INFO* pInfo)
{
	memcpy(&m_tInfo, pInfo, sizeof(OBJ_INFO));
}

void CToolStaticObject::ObjInfoSetting(void)
{
	//위치값
	m_tInfo.m_vPos = m_pInfo->m_vPos;
	//이름
	lstrcpy(m_tInfo.m_szName, m_strName.c_str());
	//크기
	m_tInfo.m_vScale = m_pInfo->m_vScale;
	//회전값
	m_tInfo.m_vAngle.x = (float)D3DXToDegree(m_pInfo->m_fAngle[ANGLE_X]);
	m_tInfo.m_vAngle.y = (float)D3DXToDegree(m_pInfo->m_fAngle[ANGLE_Y]);
	m_tInfo.m_vAngle.z = (float)D3DXToDegree(m_pInfo->m_fAngle[ANGLE_Z]);
}

void CToolStaticObject::InfoSetting(void)
{
	m_pInfo->m_vPos = m_tInfo.m_vPos;
	m_pInfo->m_vScale = m_tInfo.m_vScale;

	m_pInfo->m_fAngle[ANGLE_X] = (float)D3DXToRadian(m_tInfo.m_vAngle.x);
	m_pInfo->m_fAngle[ANGLE_Y] = (float)D3DXToRadian(m_tInfo.m_vAngle.y);
	m_pInfo->m_fAngle[ANGLE_Z] = (float)D3DXToRadian(m_tInfo.m_vAngle.z);
}
