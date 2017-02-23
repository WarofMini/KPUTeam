#include "stdafx.h"
#include "StaticObject.h"
#include "StaticMesh.h"
#include "Texture.h"
#include "Info.h"
#include "VIBuffer.h"
#include "ShaderMgr.h"
#include "Camera.h"
#include "ObjMgr.h"
#include "Shader.h"
#include "Device.h"
#include "RenderMgr.h"
#include "ResourcesMgr.h"
#include "Input.h"
#include "TimeMgr.h"


CStaticObject::CStaticObject()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;

	ZeroMemory(&m_tInfo, sizeof(OBJ_INFO));
}

CStaticObject::~CStaticObject()
{
	Release();
}

HRESULT CStaticObject::Initialize(void)
{
	if (FAILED(AddComponent()))
		return E_FAIL;

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, this);


	return S_OK;
}

int CStaticObject::Update(void)
{
	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	CObject::Update();

	return 0;
}

void CStaticObject::Render(void)
{
	ConstantBuffer cb;
	D3DXMatrixTranspose(&cb.matWorld, &m_pInfo->m_matWorld);
	D3DXMatrixTranspose(&cb.matView, &CCamera::GetInstance()->m_matView);
	D3DXMatrixTranspose(&cb.matProjection, &CCamera::GetInstance()->m_matProj);
	m_pGrapicDevice->m_pDeviceContext->UpdateSubresource(m_pBuffer->m_ConstantBuffer, 0, NULL, &cb, 0, 0);

	m_pGrapicDevice->m_pDeviceContext->VSSetShader(m_pVertexShader->m_pVertexShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->VSSetConstantBuffers(0, 1, &m_pBuffer->m_ConstantBuffer);

	//////////////////
	m_pGrapicDevice->m_pDeviceContext->PSSetShader(m_pPixelShader->m_pPixelShader, NULL, 0);
	m_pGrapicDevice->m_pDeviceContext->PSSetShaderResources(0, 1, &m_pTexture->m_pTextureRV);
	m_pGrapicDevice->m_pDeviceContext->PSSetSamplers(0, 1, &m_pTexture->m_pSamplerLinear);

	m_pBuffer->Render();
}

CStaticObject * CStaticObject::Create(wstring strName)
{
	CStaticObject* pObj = new CStaticObject;

	pObj->SetStrName(strName);

	if (FAILED(pObj->Initialize()))
		::Safe_Delete(pObj);

	return pObj;
}

void CStaticObject::Release(void)
{
	//::Safe_Delete(m_pBuffer);
	//::Safe_Delete(m_pInfo);
	//::Safe_Delete(m_pTexture);
}

HRESULT CStaticObject::AddComponent(void)
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

	//Texture
	pComponent = CResourcesMgr::GetInstance()->CloneResource(RESOURCE_STAGE, L"Texture_Couch");
	m_pTexture = dynamic_cast<CTexture*>(pComponent);
	NULL_CHECK_RETURN(m_pTexture, E_FAIL);
	m_mapComponent.insert(map<const TCHAR*, CComponent*>::value_type(L"Texture", pComponent));

	m_pVertexShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS");
	m_pPixelShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS");

	return S_OK;
}

void CStaticObject::SetStrName(wstring _strName)
{
	m_strName = _strName;
}

wstring CStaticObject::GetStrName(void)
{
	return m_strName;
}

CVIBuffer * CStaticObject::GetBuffer(void)
{
	return m_pBuffer;
}

OBJ_INFO * CStaticObject::GetObjInfo(void)
{
	return &m_tInfo;
}

void CStaticObject::SetObjInfo(OBJ_INFO * pInfo)
{
	memcpy(&m_tInfo, pInfo, sizeof(OBJ_INFO));
}

void CStaticObject::ObjInfoSetting(void)
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

void CStaticObject::InfoSetting(void)
{
	m_pInfo->m_vPos = m_tInfo.m_vPos;
	m_pInfo->m_vScale = m_tInfo.m_vScale;

	m_pInfo->m_fAngle[ANGLE_X] = (float)D3DXToRadian(m_tInfo.m_vAngle.x);
	m_pInfo->m_fAngle[ANGLE_Y] = (float)D3DXToRadian(m_tInfo.m_vAngle.y);
	m_pInfo->m_fAngle[ANGLE_Z] = (float)D3DXToRadian(m_tInfo.m_vAngle.z);
}
