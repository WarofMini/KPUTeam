#ifndef RenderTargetMgr_h__
#define RenderTargetMgr_h__

#include "Include.h"

class CRenderTexture;

class CRenderTargetMgr
{
	DECLARE_SINGLETON(CRenderTargetMgr)

private:
	explicit CRenderTargetMgr(void);
	~CRenderTargetMgr(void);

public:
	ID3D11RenderTargetView** Get_RanderTargetView(const _tchar* pRenderTargetTag);
	ID3D11DepthStencilView* Get_DepthStencilView(const _tchar* pRenderTargetTag);

	void Set_RenderTarget(const _tchar* pRenderTargetTag, const _uint& uiNumViews, _bool bDepthStencilView);
	void Set_Texture(const _tchar* pRenderTargetTag, const _uint& uiSlot, const _uint uiIndex);

public:
	HRESULT Ready_RenderTarget(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext, const _tchar* pRenderTargetTag
								, const DXGI_FORMAT& eFormat, const _ushort& wSizeX, const _ushort& wSizeY, _float fRenderPosX, _float fRenderPosY);
	void Render(const _tchar* pRenderTargetTag);
	void Release(void);

private:
	CRenderTexture* Find_RenderTarget(const _tchar* pRenderTargetTag);

private:
	typedef map<const _tchar*, CRenderTexture*>	MAPRENDERTARGETS;
	MAPRENDERTARGETS	m_mapRenderTarget;
};

#endif