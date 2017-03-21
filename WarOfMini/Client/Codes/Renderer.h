#ifndef Renderer_h__
#define Renderer_h__

#include "Include.h"

class CRcTex;
class CScene;
class CGameObject;

class CRenderer
{
public:
	enum RENDERTYPE { RENDER_PRIORITY, RENDER_ZSORT, RENDER_UI, RENDER_END, RENDER_INST, RENDER_ALPHA, RENDER_ALPHAINST };

private:
	explicit CRenderer(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);
	virtual ~CRenderer(void);

public:
	void Add_RenderGroup(RENDERTYPE eType, CGameObject* pGameObject, _float fViewZ = 0);
	void Add_RenderInstGroup(RENDERTYPE eType, _uint uiObjNum, _matrix* pMatWorld);

public:
	HRESULT Ready_Renderer(void);
	void Render(void);
	void Clear_RenderGroup(void);
	void SwapChain_Clear_RenderGroup(void);

private:
	void Render_Priority(void);
	void Render_Inst(void);
	void Render_ZSort(void);
	void Render_AlphaInst(void);
	void Render_Alpha(void);
	void Render_UI(void);

public:
	static CRenderer* Create(ID3D11Device* pGraphicDev, ID3D11DeviceContext* pContext);

public:
	void Release(void);

private:
	ID3D11Device*				m_pGraphicDev;
	ID3D11DeviceContext*		m_pContext;
	IDXGISwapChain*				m_pSwapChain;
	ID3D11RenderTargetView*		m_pRenderTargetView;
	ID3D11DepthStencilView*		m_pDepthStencilView;

private:
	typedef list<CGameObject*> RENDERLIST;
	RENDERLIST	m_RenderGroup[RENDER_END];

	typedef vector<_matrix*> VECWORLD;
	typedef map<_uint, VECWORLD> MAPINST;

	MAPINST m_mapInst;
	MAPINST m_mapAlphaInst;

	typedef multimap<_float, CGameObject*, greater<_float>> MAPALPHA;
	MAPALPHA m_mapAlpha;

private:
	CRcTex* m_pRcTex;

	//Temp;
	_bool m_bDrawNavMesh;
	_bool m_bDrawRenderTarget;
};

#endif // Renderer_h__
