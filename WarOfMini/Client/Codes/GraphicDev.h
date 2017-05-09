#ifndef GraphicDev_h__
#define GraphicDev_h__

#include "Include.h"

class CGraphicDev
{

public:
	DECLARE_SINGLETON(CGraphicDev)
	enum WINMODE { MODE_FULL, MODE_WIN };

private:
	explicit CGraphicDev(void);
			~CGraphicDev(void);

private:
	//������˰� �ڿ� �Ҵ翡 ���δ�.
	ID3D11Device*					m_pGraphicDev;
	//���� ����� ����, �ڿ��� �׷��� ������ ���ο� ����, GPU�� ������ ������ ��ɵ��� �����ϴµ� ���
	ID3D11DeviceContext*			m_pContext;

	IDXGISwapChain*					m_pSwapChain;

	//����Ÿ��
	ID3D11RenderTargetView*			m_pRenderTargetView;
	ID3D11Texture2D*				m_pDepthStencilBuffer;
	ID3D11DepthStencilView*			m_pDepthStencilView;

	//���̴�
	ID3D11Buffer*					m_pBaseShaderCB;
	ID3D11Buffer*					m_pInstShaderCB;
	ID3D11Buffer*					m_pDynamicShaderCB;
	ID3D11SamplerState*				m_pBaseSampler;


	//�����Ͷ�����
	ID3D11RasterizerState*			m_pWireFrameRS;
	ID3D11RasterizerState*			m_pSolidRS;
	ID3D11RasterizerState*			m_pNoneCullRS;
	ID3D11BlendState*				m_pAlphaBlendState;


	_bool							m_b4xMsaaCheck;//ǰ���˻�
	_ushort							m_usFPS; //���÷��� ��� ������
	_bool							m_bWireEnable;

private:
	HRESULT Set_SwapChain(DXGI_SWAP_CHAIN_DESC& sd, WINMODE eWinMode, HWND hWnd
					 , const _ushort& wSizeX, const _ushort& wSizeY);

	void Set_DepthStencil(D3D11_TEXTURE2D_DESC& td, const _ushort& wSizeX, const _ushort& wSizeY);
	
	void Set_Viewport(D3D11_VIEWPORT& vp, const _ushort& wSizeX, const _ushort& wSizeY);


public:
	void							SetAlphaEnable(_bool bAlphaEnable);
	void							SetCullEnable(_bool bCullEnable);
	void							SetWireFrame(_bool bWireEnable);
	_bool							GetWireFrame(void);

public:
	HRESULT Ready_GraphicDev(HWND hWnd, WINMODE eWinMode, const _ushort& wSizeX, const _ushort& wSizeY
							,ID3D11Device*& pGraphicDev, ID3D11DeviceContext*& pContext);

	IDXGISwapChain*					GetSwapChain();
	ID3D11RenderTargetView*			GetRenderTargetView();
	ID3D11DepthStencilView*			GetDepthStencilView();
	ID3D11Buffer*					GetBaseShaderCB();
	ID3D11Buffer*					GetInstShaderCB();
	ID3D11Buffer*					GetDynamicShaderCB();
	ID3D11SamplerState*				GetBaseSampler();
	ID3D11Device*					GetGraphicDevice();
	ID3D11DeviceContext*			GetContext();

public:
	void							ChangeScreenMode(void);

	void							OnResizeBackBuffers(void);


public:
	void							Release(void);

};











#endif

