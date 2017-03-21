#ifndef RenderTarget_h__
#define RenderTarget_h__

#include "Include.h"

class CRcTex;
class CTransform;

class CRenderTexture
{
private:
	explicit CRenderTexture(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	~CRenderTexture(void);

public:
	ID3D11RenderTargetView** Get_RanderTargetView(void);
	ID3D11DepthStencilView* Get_DepthStencilView(void);
	void Set_RenderTarget(const _uint& uiNumViews, _bool bDepthStencilView);
	void Set_Texture(const _uint& uiSlot, const _uint uiIndex);

public:
	static CRenderTexture* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext
		, const DXGI_FORMAT& eFormat, const _ushort& wSizeX, const _ushort& wSizeY, _float fRenderPosX, _float fRenderPosY);
	void Render(void);
	void Release(void);

private:
	HRESULT Ready_RenderTexture(const DXGI_FORMAT& eFormat, const _ushort& wSizeX, const _ushort& wSizeY, _float fRenderPosX, _float fRenderPosY);

private:
	ID3D11Device*			m_pGraphicDev;
	ID3D11DeviceContext*	m_pContext;

private:
	ID3D11Texture2D*			m_pRTTexture;
	ID3D11RenderTargetView*		m_pRTView[2];
	ID3D11ShaderResourceView*	m_pShaderResourceView[2];
	D3D11_TEXTURE2D_DESC		m_tTextureDesc;
	ID3D11DepthStencilView*		m_pDepthStencilView;

private:
	CTransform*		m_pTransform;
	CRcTex*			m_pRcTex;
};

#endif