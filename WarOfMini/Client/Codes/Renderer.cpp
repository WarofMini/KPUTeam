#include "stdafx.h"
#include "Renderer.h"
#include "GameObject.h"
#include "FontMgr.h"
#include "RcTex.h"
#include "GraphicDev.h"
#include "ResourcesMgr.h"
#include "RenderTargetMgr.h"
#include "ShaderMgr.h"
#include "MeshMgr.h"
#include "CameraMgr.h"

CRenderer::CRenderer(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: m_pGraphicDev(pGraphicDev)
, m_pContext(pContext)
, m_pSwapChain(NULL)
, m_pRenderTargetView(NULL)
, m_pDepthStencilView(NULL)
, m_bDrawNavMesh(FALSE)
, m_bDrawRenderTarget(FALSE)
, m_pRcTex(NULL)
{
}

CRenderer::~CRenderer(void)
{
}

void CRenderer::Add_RenderGroup(RENDERTYPE eType, CGameObject* pGameObject, _float fViewZ /*= 0*/)
{
	if (eType < RENDER_END)
		m_RenderGroup[eType].push_back(pGameObject);

	if (eType == RENDER_ALPHA)
		m_mapAlpha.insert(MAPALPHA::value_type(fViewZ, pGameObject));
}

void CRenderer::Add_RenderInstGroup(RENDERTYPE eType, UINT uiObjNum, XMFLOAT4X4* pMatWorld)
{
	if (eType != RENDER_INST && eType != RENDER_ALPHAINST)
		return;

	MAPINST* pMapInst = nullptr;

	if (eType == RENDER_INST)
		pMapInst = &m_mapInst;

	else if (eType == RENDER_ALPHAINST)
		pMapInst = &m_mapAlphaInst;

	MAPINST::iterator iter = find_if(pMapInst->begin(), pMapInst->end(), CNumFinder(uiObjNum));

	if (iter == pMapInst->end())
	{
		VECWORLD vecObjWorld;
		vecObjWorld.reserve(INSTCNT);
		vecObjWorld.push_back(pMatWorld);
		pMapInst->insert(MAPINST::value_type(uiObjNum, vecObjWorld));
	}

	else
		iter->second.push_back(pMatWorld);
}

HRESULT CRenderer::Ready_Renderer(void)
{
	
	m_pSwapChain = CGraphicDev::GetInstance()->GetSwapChain();
	m_pRenderTargetView = CGraphicDev::GetInstance()->GetRenderTargetView();
	m_pDepthStencilView = CGraphicDev::GetInstance()->GetDepthStencilView();

	m_pRcTex = dynamic_cast<CRcTex*>(CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex"));

	// Shader
	if (FAILED(CShaderMgr::GetInstance()->Ready_ShaderFromFiles(m_pGraphicDev, m_pContext, L"Shader_Default", L"../Bin/ShaderCode/Shader_Default.fx", 0)))
		return E_FAIL;

	if (FAILED(CShaderMgr::GetInstance()->Ready_ShaderFromFiles(m_pGraphicDev, m_pContext, L"Shader_Effect", L"../Bin/ShaderCode/Shader_Effect.fx", 0)))
		return E_FAIL;

	if (FAILED(CShaderMgr::GetInstance()->Ready_ShaderFromFiles(m_pGraphicDev, m_pContext, L"Shader_TitleBack", L"../Bin/ShaderCode/Shader_TitleBack.fx", 0)))
		return E_FAIL;


	if (FAILED(CShaderMgr::GetInstance()->Ready_ShaderFromFiles(m_pGraphicDev, m_pContext, L"Shader_Normal", L"../Bin/ShaderCode/Shader_Normal.fx", 0)))
		return E_FAIL;

	if (FAILED(CShaderMgr::GetInstance()->Ready_ShaderFromFiles(m_pGraphicDev, m_pContext, L"Shader_Instancing", L"../Bin/ShaderCode/Shader_Instancing.fx", 0)))
		return E_FAIL;

	if (FAILED(CShaderMgr::GetInstance()->Ready_ShaderFromFiles(m_pGraphicDev, m_pContext, L"Shader_DynamicMesh", L"../Bin/ShaderCode/Shader_DynamicMesh.fx", CShaderMgr::LAYOUT_FLAG_BONE)))
		return E_FAIL;

	if (FAILED(CShaderMgr::GetInstance()->Ready_ShaderFromFiles(m_pGraphicDev, m_pContext, L"Shader_FogResult", L"../Bin/ShaderCode/Shader_FogResult.fx", 0)))
		return E_FAIL;

	return S_OK;
}

void CRenderer::Render(void)
{
	m_pContext->ClearRenderTargetView((CRenderTargetMgr::GetInstance()->Get_RanderTargetView(L"RT_Blend"))[0], D3DXCOLOR(0.f, 0.f, 0.f, 0.f));
	m_pContext->ClearRenderTargetView((CRenderTargetMgr::GetInstance()->Get_RanderTargetView(L"RT_Blend"))[1], D3DXCOLOR(1.f, 1.f, 1.f, 0.f));
	m_pContext->ClearDepthStencilView((CRenderTargetMgr::GetInstance()->Get_DepthStencilView(L"RT_Blend")), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	m_pRenderTargetView = CGraphicDev::GetInstance()->GetRenderTargetView();
	m_pDepthStencilView = CGraphicDev::GetInstance()->GetDepthStencilView();

	m_pContext->ClearRenderTargetView(m_pRenderTargetView, D3DXCOLOR(0.f, 0.f, 1.0f, 0.f));
	m_pContext->ClearDepthStencilView(m_pDepthStencilView, D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.f, 0);

	Render_Priority();

	Render_Inst();


	Render_ZSort();

	// Temp
	//네비메시 아직 없어서 주석처리
	//if (Get_InputKeyDown(DIK_0)) m_bDrawNavMesh = !m_bDrawNavMesh;		// 네비메시 draw on off
	//if (m_bDrawNavMesh)	CNavMesh::GetInstance()->Render_NavMesh();

	//Render_AlphaInst();
	Render_Alpha();

	Render_UI();

	// Render Window
	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);


	//렌더타겟 On/ Off
	if((GetAsyncKeyState('K')) & 1) 
		m_bDrawRenderTarget = !m_bDrawRenderTarget;
	if (m_bDrawRenderTarget) 
		CRenderTargetMgr::GetInstance()->Render(L"RT_Blend");

	
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_FogResult"));

	ID3D11Buffer* pBaseShaderCB = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tBaseShaderCB;

	tBaseShaderCB.matWorld = XMMatrixTranspose(XMMatrixScaling(2.f, 2.f, 2.f));
	tBaseShaderCB.matView = XMMatrixTranspose(XMMatrixIdentity());
	tBaseShaderCB.matProj = XMMatrixTranspose(XMMatrixIdentity());

	m_pContext->UpdateSubresource(pBaseShaderCB, 0, NULL, &tBaseShaderCB, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_FogResult"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pBaseShaderCB);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_FogResult"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	for (_uint uiSlot = 0; uiSlot < 2; ++uiSlot)
		CRenderTargetMgr::GetInstance()->Set_Texture(L"RT_Blend", uiSlot, uiSlot);

	m_pRcTex->Render();

}

void CRenderer::Clear_RenderGroup(void)
{
	for (_int i = 0; i < RENDER_END; ++i)
	{
		m_RenderGroup[i].clear();
	}

	m_mapAlpha.clear();

	MAPINST::iterator	iter = m_mapAlphaInst.begin();
	MAPINST::iterator	iter_end = m_mapAlphaInst.end();

	for (; iter != iter_end; ++iter)
		iter->second.clear();

	iter = m_mapInst.begin();
	iter_end = m_mapInst.end();

	for (; iter != iter_end; ++iter)
		iter->second.clear();

	m_mapInst.clear();
}

void CRenderer::SwapChain_Clear_RenderGroup(void)
{
	m_pSwapChain->Present(1, 0);

	Clear_RenderGroup();
}

void CRenderer::Render_Priority(void)
{
	CRenderTargetMgr::GetInstance()->Set_RenderTarget(L"RT_Blend", 2, FALSE);


	RENDERLIST::iterator iter = m_RenderGroup[RENDER_PRIORITY].begin();
	RENDERLIST::iterator iter_end = m_RenderGroup[RENDER_PRIORITY].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

void CRenderer::Render_Inst(void)
{
	CRenderTargetMgr::GetInstance()->Set_RenderTarget(L"RT_Blend", 2, TRUE);

	MAPINST::iterator	iter = m_mapInst.begin();
	MAPINST::iterator	iter_end = m_mapInst.end();

	for (; iter != iter_end; ++iter)
	{
		CMeshMgr::GetInstance()->RenderInst_MeshMgr(iter->first, iter->second);
	}
}

void CRenderer::Render_ZSort(void)
{
	CRenderTargetMgr::GetInstance()->Set_RenderTarget(L"RT_Blend", 2, TRUE);

	RENDERLIST::iterator iter = m_RenderGroup[RENDER_ZSORT].begin();
	RENDERLIST::iterator iter_end = m_RenderGroup[RENDER_ZSORT].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}
}

void CRenderer::Render_AlphaInst(void)
{
	//m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, NULL);

	MAPINST::iterator	iter = m_mapAlphaInst.begin();
	MAPINST::iterator	iter_end = m_mapAlphaInst.end();

	for (; iter != iter_end; ++iter)
	{
		sort(iter->second.begin(), iter->second.end()
			, [](const XMFLOAT4X4* m1, const XMFLOAT4X4* m2)
		{
			XMMATRIX matView = XMLoadFloat4x4(CCameraMgr::GetInstance()->Get_CurCameraView());
			XMFLOAT4X4 matViewWorld1, matViewWorld2;

			XMStoreFloat4x4(&matViewWorld1, XMLoadFloat4x4(m1) * matView);
			XMStoreFloat4x4(&matViewWorld2, XMLoadFloat4x4(m2) * matView);

			return matViewWorld1._43 > matViewWorld2._43;
		});

		CMeshMgr::GetInstance()->RenderInst_MeshMgr(iter->first, iter->second);
	}

	CGraphicDev::GetInstance()->SetAlphaEnable(FALSE);
}

void CRenderer::Render_Alpha(void)
{
	CRenderTargetMgr::GetInstance()->Set_RenderTarget(L"RT_Blend", 2, TRUE);
	CGraphicDev::GetInstance()->SetAlphaEnable(TRUE);

	MAPALPHA::iterator	iter = m_mapAlpha.begin();
	MAPALPHA::iterator	iter_end = m_mapAlpha.end();

	for (; iter != iter_end; ++iter)
	{

		iter->second->Render();
	}

	CGraphicDev::GetInstance()->SetAlphaEnable(FALSE);
}

void CRenderer::Render_UI(void)
{
	CRenderTargetMgr::GetInstance()->Set_RenderTarget(L"RT_Blend", 2, TRUE);
	CGraphicDev::GetInstance()->SetAlphaEnable(TRUE);

	RENDERLIST::iterator iter = m_RenderGroup[RENDER_UI].begin();
	RENDERLIST::iterator iter_end = m_RenderGroup[RENDER_UI].end();

	for (; iter != iter_end; ++iter)
	{
		(*iter)->Render();
	}

	CGraphicDev::GetInstance()->SetAlphaEnable(FALSE);
}

CRenderer* CRenderer::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
{
	CRenderer* pRenderer = new CRenderer(pGraphicDev, pContext);

	if (FAILED(pRenderer->Ready_Renderer()))
	{
		Safe_Release(pRenderer);
	}

	return pRenderer;
}

void CRenderer::Release(void)
{
	Safe_Release(m_pRcTex);
	delete this;
}