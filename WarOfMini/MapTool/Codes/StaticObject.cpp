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

CToolStaticObject::CToolStaticObject()
{
	m_pBuffer = NULL;
	m_pVertexShader = NULL;
	m_pPixelShader = NULL;
	m_pTexture = NULL;
	m_pMouseCol = NULL;
	m_pBoundingBox = NULL;

	m_pVertexColorShader = NULL;
	m_pPixelColorShader = NULL;

	ZeroMemory(&m_tInfo, sizeof(OBJ_INFO));
}

CToolStaticObject::~CToolStaticObject()
{
	Release();
}

HRESULT CToolStaticObject::Initialize(void)
{
	if (FAILED(AddComponent()))
		return E_FAIL;

	CRenderMgr::GetInstance()->AddRenderGroup(TYPE_NONEALPHA, this);
	
	m_pMouseCol = CMouseCol::Create();

	return S_OK;
}

int CToolStaticObject::Update(void)
{
	D3DXVec3TransformNormal(&m_pInfo->m_vDir, &g_vLook, &m_pInfo->m_matWorld);

	CObj::Update();

	//오브젝트 피킹체크
	//if(m_pMouseCol->PickObjMesh((CMesh*)m_pBuffer, &m_pInfo->m_matWorld))
	//{
	//	
	//}

	//바운딩 박스 피킹
	if(m_pMouseCol->PickBoundingBox(m_pBoundingBox, &m_pInfo->m_matWorld))
	{
	}

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
	::Safe_Delete(m_pBuffer);
	::Safe_Delete(m_pInfo);
	::Safe_Delete(m_pTexture);
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

	m_pVertexColorShader = CShaderMgr::GetInstance()->Clone_Shader(L"VS_COLOR");
	m_pPixelColorShader = CShaderMgr::GetInstance()->Clone_Shader(L"PS_COLOR");


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
