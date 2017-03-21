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

	sd.BufferDesc.Width = wSizeX;//원하는 후면 버퍼 너비
	sd.BufferDesc.Height = wSizeY; //원하는 후면 버퍼 높이
	sd.BufferDesc.RefreshRate.Numerator = m_usFPS; //디스플레이 모드 갱신율
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; //후면 버퍼 픽셀 형식
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;//디스플레이 스캔라인 모드
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED; //디스클레이 비례모드

	 // 4X MSAA 품질 수준 지원 점검
	if (m_b4xMsaaCheck)
	{
		UINT ui4xMsaaQuality;

		//DXGI_FORMAT_R8G8B8A8_UNORM : 8비트 R,G,B,A 
		//일반적으로 모니터의 색상당 비트수가 24비트 이하이므로 정밀도를 그보다 높게 잡아봤자 낭비
		hr = m_pGraphicDev->CheckMultisampleQualityLevels(DXGI_FORMAT_R8G8B8A8_UNORM, 4, &ui4xMsaaQuality);

		if (FAILED(hr) == TRUE)
		{
			MSG_BOX(L"MultisampleQualityLevels Failed");
			return E_FAIL;
		}

		assert(ui4xMsaaQuality > 0);

		sd.SampleDesc.Count = 4; //다중 표본화를 위해 추출할 표본 개수와 품질 수준을 서술하는 구조체
		sd.SampleDesc.Quality = ui4xMsaaQuality - 1;
	}

	else
	{
		sd.SampleDesc.Count = 1; //스왑 체인에서 사용할 후면 버퍼개수
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = hWnd; //렌더링 결과를 표시할 창의 핸들
	sd.Windowed = eWinMode; //창모드, 전체화면모드 선택
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD; //교환 효과를 서술하는 구조체, 
	 //DISCARD를 지정시 디스플레이 구동기가 가장 효율적인 제시 방법 선택

	sd.Flags = 0; //추가적인 플래그들
				  //DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH : 지정시 응용프로그램이 전체화면 모드로 전환할때
				  //현재의 후면 버퍼 설정에 가장 잘 부합하는 디스플레이  모드 자동선택
				  //플래그 지정 안할시 그냥 현재의 데스크톱 디스플레이 모드가 사용됨

	return S_OK;
}

void CGraphicDev::Set_DepthStencil(D3D11_TEXTURE2D_DESC & td, const _ushort & wSizeX, const _ushort & wSizeY)
{
	td.Width = wSizeX; //텍스쳐 너비(텍셀 단위)
	td.Height = wSizeY; //텍스처의 높이(텍셀 단위)
	td.MipLevels = 1;   //밉맵 개수
	td.ArraySize = 1;  //텍스처 배열의 텍스처 개수
	td.Format = DXGI_FORMAT_D24_UNORM_S8_UINT; //텍스처 형식


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
	//텍스처의 용도  
	//1.DEFAULT : 자원 GPU가 일고 쓸 경우 -> CPU는 자원을 읽거나 쓸 수 없다.
	//2.IMMUTABLE :  자원을 일단 생성후 내용을 바꾸지 않는 경우 지정
	//3 DYNAMIC  : 응용프로그램(CPU)이 자원의 내용을 빈번하게 갱신할 경우 지정
	//4.STAGING : CPU가 자원의 복사본을 읽어야 할경우(느린연산)

	td.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	//자원을 파이프라인에 어떤식으로 묶을 것인지 지정
	
	td.CPUAccessFlags = 0;
	//CPU가 자원에 접근하는 방식을 결정하는 플래그
	//GPU만 읽고 쓸뿐 CPU는 전혀 접근하지 않으면 NULL

	td.MiscFlags = 0; //기타 플래그
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

	//Device 생성
	HRESULT hr = E_FAIL;

	_uint	uDeviceFlag = 0;

//디버그 모드 빌드에서 디버그 계층을 활성화 하기 위해서는 반드시 설정 필수!!
//디버그 플래그를 지정할 시 VC++ 출력 창에 디버그 메시지를 보냄	
#if defined(DEBUG) || defined(_DEBUG)
	uDeviceFlag |= D3D11_CREATE_DEVICE_DEBUG;								
#endif

	//D3D11CreateDevice : 장치와 문맥을 생성하는 함수
	//1. 디스플레이 어댑터 지정. NULL  = 기본 디스플레이 어댑터 사용
	//2. DriverType : 일반적으로 렌더링에 3차원 그래픽 가속이 적용되게 하기 위해 D3D_DRIVER_TYPE_HARDWARE 사용
	//3. SoftWare : 소프트웨어 구동기를 지정 하드웨어를 사용해서 렌더링 할경우 NULL값 지정
	//4. Flags : 추가적인 장치 생성 플래그들(OR로 결합 가능)지정
	//5. pFeatureLevels : 원소들의 배열, 원소들의 순서가 곧 기능수준들을 점검. NULL값이면 지원되는 최고 기능 수준 선택
	//6. FeatureLevels : 배열 pFeatureLevels의 D3D_FEATURE_LEVEL의 원소 개수 5번이 NULL이면  0으로 지정한다.
	//7. SDKVersion : 항상 D3D11_SDK_VERSION을 지정
	//8. ppDevice : 함수가 생성한 장치 반환
	//9. pFeatureLevel: pFeatureLevel 배열에서 처음으로 지원되는 기능을 돌려준다.
	//10. ppImmediateContext : 생성된 장치 문맥을 돌려준다.
	hr = D3D11CreateDevice(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, uDeviceFlag,
				0, 0, D3D11_SDK_VERSION, &m_pGraphicDev, NULL, &m_pContext);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"Device Created Failed");
		return E_FAIL;
	}

	IDXGIDevice* pDXGIDevice = NULL;

	//오류를 피하기 위해 장치 생성에 쓰인 IDXGIFactory 인스턴스 사용
	//COM 질의 과정============================================================================
	hr = m_pGraphicDev->QueryInterface(__uuidof(IDXGIDevice), (void**)&pDXGIDevice);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"IDXGIDevice Failed");
		return E_FAIL;
	}

	//IDXGIAdapter를 통해서 CreateSwapChain 메서드를 호출하여 교환 사슬 인터페이스 생성
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

	// Swap Chain 생성
	DXGI_SWAP_CHAIN_DESC sd;

	hr = Set_SwapChain(sd, eWinMode, hWnd, wSizeX, wSizeY);

	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"SwapChain Failed");
		return E_FAIL;
	}

	//1.ID3D11Device를 가리키는 포인터, 2. 스왑체인 구조체 포인터, 3.생성된 스왑체인 인터페이스 반환
	hr = pDXGIFactory->CreateSwapChain(m_pGraphicDev, &sd, &m_pSwapChain);
	
	if (FAILED(hr) == TRUE)
	{
		MSG_BOX(L"SwapChain Created Failed");
		return E_FAIL;
	}

	//COM 인터페이스 해제
	Safe_Release(pDXGIDevice);
	Safe_Release(pDXGIAdapter);
	Safe_Release(pDXGIFactory);


	//렌더 타겟 뷰 생성
	//반드시 자원에 대한 뷰를 생성하고 그 뷰를 파이프라인 단계에 묶어야 한다.
	//후면 버퍼를 파이프라인 출력 병합기(output merger) 단계에 묶어야 버퍼에 먼가를 그릴수 있게 된다.
	//그럴려면 후면 버퍼에 대한 렌더 대상 뷰를 생성해야한다.
	ID3D11Texture2D* pBackBuffer;
	m_pSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pBackBuffer));


	//렌더타겟뷰 생성 메서드
	//1.렌더 대상 자원, 
	//2.D3D11_RENDER_TARGET_VIEW_DESC 구조체를 가리키는 포인터
	//만약 형식을 완전히 지정해서 자원을 생성했다면 NULL지정해도 된다.
	//3.생성된 렌더 대상 뷰 반환
	m_pGraphicDev->CreateRenderTargetView(pBackBuffer, 0, &m_pRenderTargetView);
	//후면 버퍼 해제
	Safe_Release(pBackBuffer);
	//=======================================================================================


	//깊이 스텐실 버퍼 생성========================================================================
	//깊이 버퍼는 깊이 정보를 담는 2차원 텍스처
	D3D11_TEXTURE2D_DESC td;
	//D3D11_TEXTURE2D_DESC 구조체의 정보를 채워주는 함수
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

	//후면 버퍼와 깊이 스텐실 버퍼에 대한 뷰들을 생성했으니
	//이 뷰들을 파이프라인의 출력 병합기 단계에 묶는다.
	m_pContext->OMSetRenderTargets(1, &m_pRenderTargetView, m_pDepthStencilView);
	//1.묶고자 하는 렌더 대상의 개수, 2.파이프라인에 묶을 렌더 대상 뷰 포인터, 3. 스텐실 뷰 포인터

	//★ 렌더 대상 뷰는 여러개를 배열에 담아서 한 번에 묶을수 있지만, 깊이 스텐실 뷰는 하나만 묶을수 있다.


	// 뷰포트 설정=================================================
	D3D11_VIEWPORT vp;
	Set_Viewport(vp, wSizeX, wSizeY);
	//Directx3D에게 뷰포트를 알려준다.
	m_pContext->RSSetViewports(1, &vp);


	//렌더링 파이프라인에 대한 설정
	//기본도형을 형성하는 방식을 Directx3D에게 알려주는데 쓰이는 수단이 바로 기본도형 위상구조이다.
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);


	//정점버퍼 생성 : 정점들을 담는 버퍼
	D3D11_BUFFER_DESC tBufferDesc;
	ZeroMemory(&tBufferDesc, sizeof(D3D11_BUFFER_DESC));

	tBufferDesc.Usage = D3D11_USAGE_DEFAULT; //버퍼가 쓰이는 방식 GPU가 버퍼의 자원을 읽고 쓸경우 DEFAULT
	tBufferDesc.ByteWidth = sizeof(BASESHADERCB); //생성할 정점 버퍼의 크기(바이트 단위)
	tBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER; //정점 버퍼의 경우에는 D3D11_BIND_VERTEX_BUFFER를 지정

	if (FAILED(m_pGraphicDev->CreateBuffer(&tBufferDesc, NULL, &m_pBaseShaderCB)))
	{
		MSG_BOX(L"BaseShader ConstantBuffer Created Failed");
		return E_FAIL;
	}

	// 인스턴싱 정점버퍼
	tBufferDesc.ByteWidth = sizeof(INSTANCINGSHADERCB);

	if (FAILED(m_pGraphicDev->CreateBuffer(&tBufferDesc, NULL, &m_pInstShaderCB)))
	{
		MSG_BOX(L"Instancing ConstantBuffer Created Failed");
		return E_FAIL;
	}

	// 다이나믹 메시 정점버퍼
	tBufferDesc.ByteWidth = sizeof(DYNAMICSHADERCB);

	if (FAILED(m_pGraphicDev->CreateBuffer(&tBufferDesc, NULL, &m_pDynamicShaderCB)))
	{
		MSG_BOX(L"DynamicMesh ConstantBuffer Created Failed");
		return E_FAIL;
	}


	//텍스처 렌더링
	//샘플러 스테이트 오브젝트 설정
	D3D11_SAMPLER_DESC sampDesc;
	ZeroMemory(&sampDesc, sizeof(sampDesc));

	sampDesc.Filter = D3D11_FILTER_ANISOTROPIC;

	if (m_b4xMsaaCheck)
		sampDesc.MaxAnisotropy = 4;
	else
		sampDesc.MaxAnisotropy = 1;

	sampDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP; //텍스처 좌표 반복
	sampDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sampDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;

	m_pGraphicDev->CreateSamplerState(&sampDesc, &m_pBaseSampler);


	// 래스터라이즈스테이트 생성
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
