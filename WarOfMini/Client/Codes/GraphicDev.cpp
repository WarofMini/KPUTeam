#include "stdafx.h"
#include "GraphicDev.h"

IMPLEMENT_SINGLETON(CGraphicDev)

CGraphicDev::CGraphicDev(void)
: m_pGraphicDev(NULL)
, m_pContext(NULL)
, m_pSwapChain(NULL)
, m_pAlphaBlendState(NULL)
, m_pWireFrameRS(NULL)
, m_pSolidRS(NULL)
, m_pNoneCullRS(NULL)
, m_pRenderTargetView(NULL)
, m_pDepthStencilBuffer(NULL)
, m_pDepthStencilView(NULL)
, m_pBaseSampler(NULL)
, m_pBaseShaderCB(NULL)
, m_pInstShaderCB(NULL)
, m_pDynamicShaderCB(NULL)
, m_b4xMsaaCheck(true)
, m_usFPS(0)
{
}

CGraphicDev::~CGraphicDev(void)
{
}

HRESULT CGraphicDev::Set_SwapChain(DXGI_SWAP_CHAIN_DESC & sd, WINMODE eWinMode, HWND hWnd, const _ushort & wSizeX, const _ushort & wSizeY)
{
	HRESULT hr = E_FAIL;

	sd.BufferDesc.Width = wSizeX;//���ϴ� �ĸ� ���� �ʺ�
	sd.BufferDesc.Height = wSizeY; //���ϴ� �ĸ� ���� ����
	sd.BufferDesc.RefreshRate.Numerator = m_usFPS; //���÷��� ��� ������
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //�ĸ� ���� �ȼ� ����
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//���÷��� ��ĵ���� ���
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //��Ŭ���� ��ʸ��

	 // 4X MSAA ǰ�� ���� ���� ����
	if (m_b4xMsaaCheck)
	{
		UINT ui4xMsaaQuality;

		//DXGI_FORMAT_R8G8B8A8_UNORM : 8��Ʈ R,G,B,A 
		//�Ϲ������� ������� ����� ��Ʈ���� 24��Ʈ �����̹Ƿ� ���е��� �׺��� ���� ��ƺ��� ����
		hr = m_pGraphicDev->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &ui4xMsaaQuality);

		if (FAILED(hr) == TRUE)
		{
			MSG_BOX(L"MultisampleQualityLevels Failed");
			return E_FAIL;
		}

		assert(ui4xMsaaQuality > 0);

		sd.SampleDesc.Count = 4; //���� ǥ��ȭ�� ���� ������ ǥ�� ������ ǰ�� ������ �����ϴ� ����ü
		sd.SampleDesc.Quality = ui4xMsaaQuality - 1;
	}

	else
	{
		sd.SampleDesc.Count = 1; //���� ü�ο��� ����� �ĸ� ���۰���
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd; //������ ����� ǥ���� â�� �ڵ�
	sd.Windowed = eWinMode; //â���, ��üȭ���� ����
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //��ȯ ȿ���� �����ϴ� ����ü, 
	 //DISCARD�� ������ ���÷��� �����Ⱑ ���� ȿ������ ���� ��� ����

	sd.Flags = 0; //�߰����� �÷��׵�
				  //DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : ������ �������α׷��� ��üȭ�� ���� ��ȯ�Ҷ�
				  //������ �ĸ� ���� ������ ���� �� �����ϴ� ���÷���  ��� �ڵ�����
				  //�÷��� ���� ���ҽ� �׳� ������ ����ũ�� ���÷��� ��尡 ����

	return S_OK;
}

void CGraphicDev::Set_DepthStencil(D3D11_TEXTURE2D_DESC & td, const _ushort & wSizeX, const _ushort & wSizeY)
{
	td.Width = wSizeX; //�ؽ��� �ʺ�(�ؼ� ����)
	td.Height = wSizeY; //�ؽ�ó�� ����(�ؼ� ����)
	td.MipLevels = 1;   //�Ӹ� ����
	td.ArraySize = 1;  //�ؽ�ó �迭�� �ؽ�ó ����
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //�ؽ�ó ����


	if (m_b4xMsaaCheck)
	{
		UINT ui4xMsaaQuality;

		m_pGraphicDev->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &ui4xMsaaQuality);
		assert(ui4xMsaaQuality > 0);

		td.SampleDesc.Count = 4;
		td.SampleDesc.Quality = ui4xMsaaQuality - 1;
	}
	else
	{
		td.SampleDesc.Count = 1;
		td.SampleDesc.Quality = 0;
	}

	td.Usage = D3D11_USAGE_DEFAULT;
	//�ؽ�ó�� �뵵  
	//1.DEFAULT : �ڿ� GPU�� �ϰ� �� ��� -> CPU�� �ڿ��� �аų� �� �� ����.
	//2.IMMUTABLE :  �ڿ��� �ϴ� ������ ������ �ٲ��� �ʴ� ��� ����
	//3 DYNAMIC  : �������α׷�(CPU)�� �ڿ��� ������ ����ϰ� ������ ��� ����
	//4.STAGING : CPU�� �ڿ��� ���纻�� �о�� �Ұ��(��������)

	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//�ڿ��� ���������ο� ������� ���� ������ ����
	
	td.CPUAccessFlags = 0;
	//CPU�� �ڿ��� �����ϴ� ����� �����ϴ� �÷���
	//GPU�� �а� ���� CPU�� ���� �������� ������ NULL

	td.MiscFlags = 0; //��Ÿ �÷���
}

void CGraphicDev::Set_Viewport(D3D11_VIEWPORT & vp, const _ushort & wSizeX, const _ushort & wSizeY)
{
	vp.TopLeftX = 0.f;
	vp.TopLeftY = 0.f;
	vp.Width = (_float)wSizeX;
	vp.Height = (_float)wSizeY;
	vp.MinDepth = 0.f;
	vp.MaxDepth = 1.f;
}

void CGraphicDev::SetAlphaEnable(_bool bAlphaEnable)
{
	if(bAlphaEnable)
		m_pContext->OMSetBlendState(m_pAlphaBlendState, NULL, 0xFFFFFF);
	else
		m_pContext->OMSetBlendState(NULL, NULL, 0xFFFFFF);
}

void CGraphicDev::SetCullEnable(_bool bCullEnable)
{
	if (bCullEnable)
		m_pContext->RSSetState(m_pSolidRS);
	else
		m_pContext->RSSetState(m_pNoneCullRS);
}

void CGraphicDev::SetWireFrame(_bool bWireEnable)
{
	if (bWireEnable)
		m_pContext->RSSetState(m_pWireFrameRS);
	else
		m_pContext->RSSetState(m_pSolidRS);
}

HRESULT CGraphicDev::Ready_GraphicDev(HWND hWnd, WINMODE eWinMode, const _ushort & wSizeX, const _ushort & wSizeY, ID3D11Device *& pGraphicDev, ID3D11DeviceContext *& pContext)
{

	//Device ����
	HRESULT hr = E_FAIL;

	_uint	uDeviceFlag = 0;

//����� ��� ���忡�� ����� ������ Ȱ��ȭ �ϱ� ���ؼ��� �ݵ�� ���� �ʼ�!!
//����� �÷��׸� ������ �� VC++ ��� â�� ����� �޽����� ����	
#if defined(DEBUG) || defined(_DEBUG)
	uDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;								
#endif

	//D3D11CreateDevice : ��ġ�� ������ �����ϴ� �Լ�
	//1. ���÷��� ����� ����. NULL  = �⺻ ���÷��� ����� ���
	//2. DriverType : �Ϲ������� �������� 3���� �׷��� ������ ����ǰ� �ϱ� ���� D3D_DRIVER_TYPE_HARDWARE ���
	//3. SoftWare : ����Ʈ���� �����⸦ ���� �ϵ��� ����ؼ� ������ �Ұ�� NULL�� ����
	//4. Flags : �߰����� ��ġ ���� �÷��׵�(OR�� ���� ����)����
	//5. pFeatureLevels : ���ҵ��� �迭, ���ҵ��� ������ �� ��ɼ��ص��� ����. NULL���̸� �����Ǵ� �ְ� ��� ���� ����
	//6. FeatureLevels : �迭 pFeatureLevels�� D3D_FEATURE_LEVEL�� ���� ���� 5���� NULL�̸�  0���� �����Ѵ�.
	//7. SDKVersion : �׻� D3D11_SDK_VERSION�� ����
	//8. ppDevice : �Լ��� ������ ��ġ ��ȯ
	//9. pFeatureLevel: pFeatureLevel �迭���� ó������ �����Ǵ� ����� �����ش�.
	//10. ppImmediateContext : ������ ��ġ ������ �����ش�.
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, uDeviceFlag,
				0, 0, D3D11_SDK_VERSION, &m_pGraphicDev, NULL, &m_pContext);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"Device Created Failed");
		return E_FAIL;
	}

	IDXGIDevice* pDXGIDevice = NULL;

	//������ ���ϱ� ���� ��ġ ������ ���� IDXGIFactory �ν��Ͻ� ���
	//COM ���� ����============================================================================
	hr = m_pGraphicDev->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"IDXGIDevice Failed");
		return E_FAIL;
	}

	//IDXGIAdapter�� ���ؼ� CreateSwapChain �޼��带 ȣ���Ͽ� ��ȯ �罽 �������̽� ����
	IDXGIAdapter* pDXGIAdapter = NULL;

	hr = pDXGIDevice->GetParent(__uuidof(IDXGIAdapter), (void**)&pDXGIAdapter);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"IDXGIAdapter Get Failed");
		return E_FAIL;
	}

	IDXGIFactory* pDXGIFactory = NULL;

	hr = pDXGIAdapter->GetParent(__uuidof(IDXGIFactory), (void**)&pDXGIFactory);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"IDXGIFactory Get Failed");
		return E_FAIL;
	}
	//========================================================================================

	// Swap Chain ����
	DXGI_SWAP_CHAIN_DESC sd;

	hr = Set_SwapChain(sd, eWinMode, hWnd, wSizeX, wSizeY);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"SwapChain Failed");
		return E_FAIL;
	}

	//1.ID3D11Device�� ����Ű�� ������, 2. ����ü�� ����ü ������, 3.������ ����ü�� �������̽� ��ȯ
	hr = pDXGIFactory->CreateSwapChain(m_pGraphicDev, &sd, &m_pSwapChain);
	
	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"SwapChain Created Failed");
		return E_FAIL;
	}

	//COM �������̽� ����
	Safe_Release(pDXGIDevice);
	Safe_Release(pDXGIAdapter);
	Safe_Release(pDXGIFactory);


	//���� Ÿ�� �� ����
	//�ݵ�� �ڿ��� ���� �並 �����ϰ� �� �並 ���������� �ܰ迡 ����� �Ѵ�.
	//�ĸ� ���۸� ���������� ��� ���ձ�(output merger) �ܰ迡 ����� ���ۿ� �հ��� �׸��� �ְ� �ȴ�.
	//�׷����� �ĸ� ���ۿ� ���� ���� ��� �並 �����ؾ��Ѵ�.
	ID3D11Texture2D* pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));


	//����Ÿ�ٺ� ���� �޼���
	//1.���� ��� �ڿ�, 
	//2.D3D11_RENDER_TARGET_VIEW_DESC ����ü�� ����Ű�� ������
	//���� ������ ������ �����ؼ� �ڿ��� �����ߴٸ� NULL�����ص� �ȴ�.
	//3.������ ���� ��� �� ��ȯ
	m_pGraphicDev->CreateRenderTargetView(pBackBuffer, 0, &m_pRenderTargetView);
	//�ĸ� ���� ����
	Safe_Release(pBackBuffer);
	//=======================================================================================


	//���� ���ٽ� ���� ����========================================================================
	//���� ���۴� ���� ������ ��� 2���� �ؽ�ó
	D3D11_TEXTURE2D_DESC td;
	//D3D11_TEXTURE2D_DESC ����ü�� ������ ä���ִ� �Լ�
	Set_DepthStencil(td, wSizeX, wSizeY);

	if (FAILED(m_pGraphicDev->CreateTexture2D(&td, 0, &m_pDepthStencilBuffer)))
	{
		MSG_BOX(L"DepthStenciBuffer Created Failed");
		return E_FAIL;
	}

	if (FAILED(m_pGraphicDev->CreateDepthStencilView(m_pDepthStencilBuffer, 0, &m_pDepthStencilView)))
	{
		MSG_BOX(L"DepthStencilView Created Failed");
		return E_FAIL;
	}

	//�ĸ� ���ۿ� ���� ���ٽ� ���ۿ� ���� ����� ����������
	//�� ����� ������������ ��� ���ձ� �ܰ迡 ���´�.
	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	//1.������ �ϴ� ���� ����� ����, 2.���������ο� ���� ���� ��� �� ������, 3. ���ٽ� �� ������

	//�� ���� ��� ��� �������� �迭�� ��Ƽ� �� ���� ������ ������, ���� ���ٽ� ��� �ϳ��� ������ �ִ�.


	// ����Ʈ ����=================================================
	D3D11_VIEWPORT vp;
	Set_Viewport(vp, wSizeX, wSizeY);
	//Directx3D���� ����Ʈ�� �˷��ش�.
	m_pContext->RSSetViewports(1, &vp);


	//������ ���������ο� ���� ����
	//�⺻������ �����ϴ� ����� Directx3D���� �˷��ִµ� ���̴� ������ �ٷ� �⺻���� �������̴�.
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//�������� ���� : �������� ��� ����
	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

	tBufferDesc.Usage = D3D11_USAGE_DEFAULT; //���۰� ���̴� ��� GPU�� ������ �ڿ��� �а� ����� DEFAULT
	tBufferDesc.ByteWidth = sizeof(BASESHADERCB); //������ ���� ������ ũ��(����Ʈ ����)
	tBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //���� ������ ��쿡�� D3D11_BIND_VERTEX_BUFFER�� ����

	if (FAILED(m_pGraphicDev->CreateBuffer(&tBufferDesc, NULL, &m_pBaseShaderCB)))
	{
		MSG_BOX(L"BaseShader ConstantBuffer Created Failed");
		return E_FAIL;
	}

	// �ν��Ͻ� ��������
	tBufferDesc.ByteWidth = sizeof(INSTANCINGSHADERCB);

	if (FAILED(m_pGraphicDev->CreateBuffer(&tBufferDesc, NULL, &m_pInstShaderCB)))
	{
		MSG_BOX(L"Instancing ConstantBuffer Created Failed");
		return E_FAIL;
	}

	// ���̳��� �޽� ��������
	tBufferDesc.ByteWidth = sizeof(DYNAMICSHADERCB);

	if (FAILED(m_pGraphicDev->CreateBuffer(&tBufferDesc, NULL, &m_pDynamicShaderCB)))
	{
		MSG_BOX(L"DynamicMesh ConstantBuffer Created Failed");
		return E_FAIL;
	}


	//�ؽ�ó ������
	//���÷� ������Ʈ ������Ʈ ����
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));

	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;

	if (m_b4xMsaaCheck)
		sampDesc.MaxAnisotropy = 4;
	else
		sampDesc.MaxAnisotropy = 1;

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //�ؽ�ó ��ǥ �ݺ�
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	m_pGraphicDev->CreateSamplerState(&sampDesc, &m_pBaseSampler);


	// �����Ͷ��������Ʈ ����
	D3D11_RASTERIZER_DESC tRasterizerDesc;
	ZeroMemory(&tRasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));

	//SolidFrame
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	tRasterizerDesc.CullMode = D3D11_CULL_BACK;
	tRasterizerDesc.DepthClipEnable = TRUE;

	if (FAILED(m_pGraphicDev->CreateRasterizerState(&tRasterizerDesc, &m_pSolidRS)))
	{
		MSG_BOX(L"RasterizerState Created Failed");
		return E_FAIL;
	}


	//WireFrame
	tRasterizerDesc.FillMode = D3D11_FILL_WIREFRAME;

	if (FAILED(m_pGraphicDev->CreateRasterizerState(&tRasterizerDesc, &m_pWireFrameRS)))
	{
		MSG_BOX(L"RasterizerState Created Failed");
		return E_FAIL;
	}

	// None Cull
	tRasterizerDesc.FillMode = D3D11_FILL_SOLID;
	tRasterizerDesc.CullMode = D3D11_CULL_NONE;

	if (FAILED(m_pGraphicDev->CreateRasterizerState(&tRasterizerDesc, &m_pNoneCullRS)))
	{
		MSG_BOX(L"RasterizerState Created Failed");
		return E_FAIL;
	}

	m_pContext->RSSetState(m_pSolidRS);


	// Create BlendState
	D3D11_BLEND_DESC tBSDesc;
	ZeroMemory(&tBSDesc, sizeof(D3D11_BLEND_DESC));

	tBSDesc.RenderTarget[0].BlendEnable = TRUE;
	tBSDesc.RenderTarget[0].SrcBlend = D3D11_BLEND_SRC_ALPHA;
	tBSDesc.RenderTarget[0].DestBlend = D3D11_BLEND_INV_SRC_ALPHA;
	tBSDesc.RenderTarget[0].BlendOp = D3D11_BLEND_OP_ADD;
	tBSDesc.RenderTarget[0].SrcBlendAlpha = D3D11_BLEND_ONE;
	tBSDesc.RenderTarget[0].DestBlendAlpha = D3D11_BLEND_ZERO;
	tBSDesc.RenderTarget[0].BlendOpAlpha = D3D11_BLEND_OP_ADD;
	tBSDesc.RenderTarget[0].RenderTargetWriteMask = 0x0F;


	if (FAILED(m_pGraphicDev->CreateBlendState(&tBSDesc, &m_pAlphaBlendState)))
	{
		MSG_BOX(L"BlendState Created Failed");
		return E_FAIL;
	}

	pGraphicDev = m_pGraphicDev;
	pContext = m_pContext;

	return S_OK;
}

IDXGISwapChain * CGraphicDev::GetSwapChain()
{
	return m_pSwapChain;
}

ID3D11RenderTargetView * CGraphicDev::GetRenderTargetView()
{
	return m_pRenderTargetView;
}

ID3D11DepthStencilView * CGraphicDev::GetDepthStencilView()
{
	return m_pDepthStencilView;
}

ID3D11Buffer * CGraphicDev::GetBaseShaderCB()
{
	return m_pBaseShaderCB;
}

ID3D11Buffer * CGraphicDev::GetInstShaderCB()
{
	return m_pInstShaderCB;
}

ID3D11Buffer * CGraphicDev::GetDynamicShaderCB()
{
	return m_pDynamicShaderCB;
}

ID3D11SamplerState * CGraphicDev::GetBaseSampler()
{
	return m_pBaseSampler;
}

ID3D11Device * CGraphicDev::GetGraphicDevice()
{
	return m_pGraphicDev;
}

ID3D11DeviceContext * CGraphicDev::GetContext()
{
	return m_pContext;
}

void CGraphicDev::Release(void)
{
	if (Safe_Com_Release(m_pNoneCullRS))
		MSG_BOX(L"m_pNoneCullRS Release Failed");

	if (Safe_Com_Release(m_pSolidRS))
		MSG_BOX(L"m_pSolidRS Release Failed");

	if (Safe_Com_Release(m_pWireFrameRS))
		MSG_BOX(L"m_pWireFrameRS Release Failed");

	if (Safe_Com_Release(m_pAlphaBlendState))
		MSG_BOX(L"m_pAlphaBlendState Release Failed");

	if (Safe_Com_Release(m_pBaseSampler))
		MSG_BOX(L"m_pBaseSampler Release Failed");

	if (Safe_Com_Release(m_pDynamicShaderCB))
		MSG_BOX(L"m_pDynamicShaderCB Release Failed");

	if (Safe_Com_Release(m_pInstShaderCB))
		MSG_BOX(L"m_pInstShaderCB Release Failed");

	if (Safe_Com_Release(m_pBaseShaderCB))
		MSG_BOX(L"m_pBaseShaderCB Release Failed");

	if (Safe_Com_Release(m_pRenderTargetView))
		MSG_BOX(L"m_pRenderTargetView Release Failed");

	if (Safe_Com_Release(m_pDepthStencilBuffer))
		MSG_BOX(L"m_pDepthStencilBuffer Release Failed");

	if (Safe_Com_Release(m_pDepthStencilView))
		MSG_BOX(L"m_pDepthStencilView Release Failed");

	if (Safe_Com_Release(m_pContext))
		MSG_BOX(L"m_pContext Release Failed");

	if (Safe_Com_Release(m_pSwapChain))
		MSG_BOX(L"m_pSwapChain Release Failed");

	if (Safe_Com_Release(m_pGraphicDev))
	{
		MSG_BOX(L"m_pGraphicDev Release Failed");
	}
		delete this;
}
