#include "stdafx.h"
#include "RenderTarget.h"
#include "Transform.h"
#include "RenderTargetMgr.h"
#include "ResourcesMgr.h"
#include "ShaderMgr.h"
#include "GraphicDev.h"

CRenderTexture::CRenderTexture(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext)
: m_pGraphicDev(pGraphicDev)
, m_pContext(pContext)
, m_pTransform(nullptr)
, m_pRcTex(nullptr)
, m_pRTTexture(nullptr)
, m_pDepthStencilView(nullptr)
{
}

CRenderTexture::~CRenderTexture(void)
{
}

void CRenderTexture::Set_RenderTarget(const _uint& uiNumViews, _bool bDepthStencilView)
{
	if (bDepthStencilView)
		m_pContext->OMSetRenderTargets(uiNumViews, m_pRTView, m_pDepthStencilView);

	else
		m_pContext->OMSetRenderTargets(uiNumViews, m_pRTView, nullptr);
}

void CRenderTexture::Set_Texture(const _uint& uiSlot, const _uint uiIndex)
{
	m_pContext->PSSetShaderResources(uiSlot, 1, &m_pShaderResourceView[uiIndex]);
}

CRenderTexture* CRenderTexture::Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
	, const DXGI_FORMAT& eFormat, const _ushort& wSizeX, const _ushort& wSizeY, _float fRenderPosX, _float fRenderPosY)
{
	CRenderTexture* pRenderTexture = new CRenderTexture(pGraphicDev, pContext);

	if (FAILED(pRenderTexture->Ready_RenderTexture(eFormat, wSizeX, wSizeY, fRenderPosX, fRenderPosY)))
	{
		MSG_BOX(L"CRenderTarget Ready_RenderTexture Failed");
		Safe_Release(pRenderTexture);
	}

	return pRenderTexture;
}

HRESULT CRenderTexture::Ready_RenderTexture(const DXGI_FORMAT& eFormat, const _ushort& wSizeX, const _ushort& wSizeY, _float fRenderPosX, _float fRenderPosY)
{
	D3D11_RENDER_TARGET_VIEW_DESC tRenderTargetViewDesc;
	D3D11_SHADER_RESOURCE_VIEW_DESC tShaderResourceViewDesc;

	// Create Texture2D
	ZeroMemory(&m_tTextureDesc, sizeof(D3D11_TEXTURE2D_DESC));

	m_tTextureDesc.Width = wSizeX;
	m_tTextureDesc.Height = wSizeY;
	m_tTextureDesc.MipLevels = 1;
	m_tTextureDesc.ArraySize = 2;
	m_tTextureDesc.Format = eFormat;
	m_tTextureDesc.SampleDesc.Count = 1;
	m_tTextureDesc.Usage = D3D11_USAGE_DEFAULT;
	m_tTextureDesc.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	if (FAILED(m_pGraphicDev->CreateTexture2D(&m_tTextureDesc, NULL, &m_pRTTexture)))
	{
		MSG_BOX(L"RenderTarget - CreateTexture2D Failed");
		return E_FAIL;
	}

	// Create RenderTargetView
	tRenderTargetViewDesc.Format = m_tTextureDesc.Format;
	tRenderTargetViewDesc.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2DARRAY;
	tRenderTargetViewDesc.Texture2DArray.ArraySize = 1;
	tRenderTargetViewDesc.Texture2DArray.MipSlice = 0;

	tRenderTargetViewDesc.Texture2DArray.FirstArraySlice = 0;
	if (FAILED(m_pGraphicDev->CreateRenderTargetView(m_pRTTexture, &tRenderTargetViewDesc, &m_pRTView[0])))
	{
		MSG_BOX(L"RenderTarget - CreateRenderTargetView Failed");
		return E_FAIL;
	}

	tRenderTargetViewDesc.Texture2DArray.FirstArraySlice = 1;
	if (FAILED(m_pGraphicDev->CreateRenderTargetView(m_pRTTexture, &tRenderTargetViewDesc, &m_pRTView[1])))
	{
		MSG_BOX(L"RenderTarget - CreateRenderTargetView Failed");
		return E_FAIL;
	}

	// Create ShaderResourceView
	tShaderResourceViewDesc.Format = m_tTextureDesc.Format;
	tShaderResourceViewDesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2DARRAY;
	tShaderResourceViewDesc.Texture2DArray.ArraySize = 1;
	tShaderResourceViewDesc.Texture2DArray.MipLevels = 1;
	tShaderResourceViewDesc.Texture2DArray.MostDetailedMip = 0;

	tShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 0;
	if (FAILED(m_pGraphicDev->CreateShaderResourceView(m_pRTTexture, &tShaderResourceViewDesc, &m_pShaderResourceView[0])))
	{
		MSG_BOX(L"RenderTarget - CreateShaderResourceView Failed");
		return E_FAIL;
	}

	tShaderResourceViewDesc.Texture2DArray.FirstArraySlice = 1;
	if (FAILED(m_pGraphicDev->CreateShaderResourceView(m_pRTTexture, &tShaderResourceViewDesc, &m_pShaderResourceView[1])))
	{
		MSG_BOX(L"RenderTarget - CreateShaderResourceView Failed");
		return E_FAIL;
	}

	// Depth
	CD3D11_TEXTURE2D_DESC dtDesc;
	dtDesc.Width = wSizeX;
	dtDesc.Height = wSizeY;
	dtDesc.MipLevels = 1;
	dtDesc.ArraySize = 1;
	dtDesc.SampleDesc.Count = 1;
	dtDesc.SampleDesc.Quality = 0;
	dtDesc.Format = DXGI_FORMAT_D32_FLOAT;
	dtDesc.Usage = D3D11_USAGE_DEFAULT;
	dtDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	dtDesc.CPUAccessFlags = 0;
	dtDesc.MiscFlags = 0;

	ID3D11Texture2D* pDepthTex = nullptr;
	if (FAILED(m_pGraphicDev->CreateTexture2D(&dtDesc, NULL, &pDepthTex)))
	{
		MSG_BOX(L"RenderTarget - CreateTexture2D Failed");
		return E_FAIL;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC dsvDesc;
	dsvDesc.Format = dtDesc.Format;
	dsvDesc.Flags = 0;
	dsvDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	dsvDesc.Texture2D.MipSlice = 0;
	if (FAILED(m_pGraphicDev->CreateDepthStencilView(pDepthTex, &dsvDesc, &m_pDepthStencilView)))
	{
		MSG_BOX(L"RenderTarget - CreateDepthStencilView Failed");
		return E_FAIL;
	}

	Safe_Release(pDepthTex);

	// Render RT
	m_pRcTex = dynamic_cast<CRcTex*>(CResourcesMgr::GetInstance()->Clone_ResourceMgr(RESOURCE_STAGE, L"Buffer_RcTex"));
	m_pTransform = CTransform::Create();

	m_pTransform->m_vPos = XMFLOAT3(fRenderPosX, fRenderPosY, 0.f);
	m_pTransform->m_vScale = XMFLOAT3(0.3f, 0.3f, 0.f);

	m_pTransform->Update(0.f);

	return S_OK;
}

void CRenderTexture::Render(void)
{
	m_pContext->IASetInputLayout(CShaderMgr::GetInstance()->Get_InputLayout(L"Shader_Default"));

	ID3D11Buffer* pConstantBuffer = CGraphicDev::GetInstance()->GetBaseShaderCB();
	ID3D11SamplerState* pBaseSampler = CGraphicDev::GetInstance()->GetBaseSampler();

	BASESHADER_CB tConstantBuffer;

	tConstantBuffer.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	tConstantBuffer.matView = XMMatrixTranspose(XMMatrixIdentity());
	tConstantBuffer.matProj = XMMatrixTranspose(XMMatrixIdentity());


	m_pContext->UpdateSubresource(pConstantBuffer, 0, NULL, &tConstantBuffer, 0, 0);

	m_pContext->VSSetShader(CShaderMgr::GetInstance()->Get_VertexShader(L"Shader_Default"), NULL, 0);
	m_pContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
	m_pContext->PSSetShader(CShaderMgr::GetInstance()->Get_PixelShader(L"Shader_Default"), NULL, 0);
	m_pContext->PSSetSamplers(0, 1, &pBaseSampler);

	// Target1
	m_pContext->PSSetShaderResources(0, 1, &m_pShaderResourceView[0]);	// RT Texture
	m_pRcTex->Render();

	// Target2
	m_pTransform->m_matWorld._42 -= 0.3f;
	tConstantBuffer.matWorld = XMMatrixTranspose(XMLoadFloat4x4(&m_pTransform->m_matWorld));
	m_pContext->UpdateSubresource(pConstantBuffer, 0, NULL, &tConstantBuffer, 0, 0);

	m_pContext->PSSetShaderResources(0, 1, &m_pShaderResourceView[1]);	// RT Texture
	m_pRcTex->Render();

	m_pTransform->m_matWorld._42 += 0.3f;
}

void CRenderTexture::Release(void)
{
	Safe_Release(m_pRcTex);
	Safe_Release(m_pTransform);

	if (Safe_Com_Release(m_pRTTexture))
		MSG_BOX(L"m_pRTTexture Release Failed");

	if (Safe_Com_Release(m_pRTView[0]))
		MSG_BOX(L"m_pRTView Release Failed");

	if (Safe_Com_Release(m_pRTView[1]))
		MSG_BOX(L"m_pRTView Release Failed");

	if (Safe_Com_Release(m_pShaderResourceView[0]))
		MSG_BOX(L"m_pShaderResourceView Release Failed");

	if (Safe_Com_Release(m_pShaderResourceView[1]))
		MSG_BOX(L"m_pShaderResourceView Release Failed");

	if (Safe_Com_Release(m_pDepthStencilView))
		MSG_BOX(L"m_pDepthStencilView Release Failed");

	delete this;
}

ID3D11RenderTargetView** CRenderTexture::Get_RanderTargetView(void)
{
	return m_pRTView;
}

ID3D11DepthStencilView* CRenderTexture::Get_DepthStencilView(void)
{
	return m_pDepthStencilView;
}
