#include "stdafx.h"
#include "cDeviceManager.h"




cDeviceManager::cDeviceManager(void)
{
	m_swapChain = 0;
	m_device = 0;
	m_deviceContext = 0;
	m_renderTargetView = 0;
	m_depthStencilBuffer = 0;
	m_depthStencilState = 0;
	m_depthStencilView = 0;
	m_rasterState = 0;
}

cDeviceManager::~cDeviceManager(void)
{

}

bool cDeviceManager::CreateDevice()
{
	RECT rc;
	GetClientRect(g_hwnd, &rc);
	int screenWidth = rc.right;
	int screenHeight = rc.bottom;
	bool vsync = true;
	HWND hwnd = g_hwnd;
	bool fullscreen = false;
	float screenDepth = 1000.f;
	float screenNear = 1.f;


	HRESULT result;
	IDXGIFactory* factory;
	IDXGIAdapter* adapter;
	IDXGIOutput* adapterOutput;
	unsigned int numModes, i, numerator, denominator;
	size_t stringLength;
	DXGI_MODE_DESC* displayModeList;
	DXGI_ADAPTER_DESC adapterDesc;
	int error;
	DXGI_SWAP_CHAIN_DESC swapChainDesc;
	D3D_FEATURE_LEVEL featureLevel;
	ID3D11Texture2D* backBufferPtr;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	D3D11_RASTERIZER_DESC rasterDesc;
	D3D11_VIEWPORT viewport;
	float fieldOfView, screenAspect;


	// vsync(수직동기화) 설정을 저장합니다.
	m_vsync_enabled = vsync;

	// DirectX 그래픽 인터페이스 팩토리를 만듭니다.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// 팩토리 객체를 사용하여 첫번째 그래픽 카드 인터페이스에 대한 아답터를 만듭니다.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// 출력(모니터)에 대한 첫번째 아답터를 나열합니다.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// DXGI_FORMAT_R8G8B8A8_UNORM 모니터 출력 디스플레이 포맷에 맞는 모드의 개수를 구합니다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// 가능한 모든 모니터와 그래픽카드 조합을 저장할 리스트를 생성합니다.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// 디스플레이 모드에 대한 리스트 구조를 채워넣습니다.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// 이제 모든 디스플레이 모드에 대해 화면 너비/높이에 맞는 디스플레이 모드를 찾습니다.
	// 적합한 것을 찾으면 모니터의 새로고침 비율의 분모와 분자 값을 저장합니다.
	for (i = 0; i < numModes; i++)
	{
		if (displayModeList[i].Width == (unsigned int)screenWidth)
		{
			if (displayModeList[i].Height == (unsigned int)screenHeight)
			{
				numerator = displayModeList[i].RefreshRate.Numerator;
				denominator = displayModeList[i].RefreshRate.Denominator;
			}
		}
	}

	// 아답터(그래픽카드)의 description을 가져옵니다.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// 현재 그래픽카드의 메모리 용량을 메가바이트 단위로 저장합니다.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// 그래픽카드의 이름을 char형 문자열 배열로 바꾼 뒤 저장합니다.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// 디스플레이 모드 리스트의 할당을 해제합니다.
	delete[] displayModeList;
	displayModeList = 0;

	// 출력 아답터를 할당 해제합니다.
	adapterOutput->Release();
	adapterOutput = 0;

	// 아답터를 할당 해제합니다.
	adapter->Release();
	adapter = 0;

	// 팩토리 객체를 할당 해제합니다.
	factory->Release();
	factory = 0;

	// 스왑 체인 description을 초기화합니다.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// 하나의 백버퍼만을 사용하도록 합니다.
	swapChainDesc.BufferCount = 1;

	// 백버퍼의 너비와 높이를 설정합니다.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// 백버퍼로 일반적인 32bit의 서페이스를 지정합니다.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// 백버퍼의 새로고침 비율을 설정합니다.
	if (m_vsync_enabled)
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = numerator;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = denominator;
	}
	else
	{
		swapChainDesc.BufferDesc.RefreshRate.Numerator = 0;
		swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	}

	// 백버퍼의 용도를 설정합니다.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// 렌더링이 이루어질 윈도우의 핸들을 설정합니다.
	swapChainDesc.OutputWindow = hwnd;

	// 멀티샘플링을 끕니다.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// 윈도우 모드 또는 풀스크린 모드를 설정합니다.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// 스캔라인의 정렬과 스캔라이닝을 지정되지 않음으로(unspecified) 설정합니다.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// 출력된 이후의 백버퍼의 내용을 버리도록 합니다.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// 추가 옵션 플래그를 사용하지 않습니다.
	swapChainDesc.Flags = 0;

	// 피쳐 레벨을 DirectX 11로 설정합니다.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// 스왑 체인, Direct3D 디바이스, Direct3D 디바이스 컨텍스트를 생성합니다.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// 백버퍼의 포인터를 가져옵니다.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// 백버퍼의 포인터로 렌더타겟 뷰를 생성합니다.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// 백버퍼 포인터를 더이상 사용하지 않으므로 할당 해제합니다.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// 깊이 버퍼의 description을 초기화합니다.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// 깊이 버퍼의 description을 작성합니다.
	depthBufferDesc.Width = screenWidth;
	depthBufferDesc.Height = screenHeight;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// description을 사용하여 깊이 버퍼의 텍스쳐를 생성합니다.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}
	
	// 스텐실 상태의 description을 초기화합니다.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// 스텐실 상태의 description을 작성합니다.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;

	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// 깊이-스텐실 상태를 생성합니다.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// 깊이-스텐실 상태를 설정합니다.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// 깊이-스텐실 뷰의 description을 초기화합니다.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// 깊이-스텐실 뷰의 description을 작성합니다.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// 깊이-스텐실 뷰를 생성합니다.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// 렌더타겟 뷰와 깊이-스텐실 버퍼를 각각 출력 파이프라인에 바인딩합니다.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


	// 어떤 도형을 어떻게 그릴 것인지 결정하는 래스터화기 description을 작성합니다.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// 작성한 description으로부터 래스터화기 상태를 생성합니다.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// 래스터화기 상태를 설정합니다.
	m_deviceContext->RSSetState(m_rasterState);

	// 렌더링을 위한 뷰포트를 설정합니다.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// 뷰포트를 생성합니다.
	m_deviceContext->RSSetViewports(1, &viewport);


	// 투영 행렬을 설정합니다.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D 렌더링을 위한 투영 행렬을 생성합니다.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);


	// 월드 행렬을 단위 행렬로 초기화합니다.
	D3DXMatrixIdentity(&m_worldMatrix);



	// 2D 렌더링에 사용될 정사영 행렬을 생성합니다.
	D3DXMatrixOrthoLH(&m_orthoMatrix, (float)screenWidth, (float)screenHeight, screenNear, screenDepth);

	return true;
}


//	UINT nCreateDeviceFlags = 0;
//
//#if defined(DEBUG) || defined(_DEBUG)
//	nCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
//#endif 
//
//	D3D_FEATURE_LEVEL			eFeatureLevel;
//	HRESULT						hr = D3D11CreateDevice(
//		NULL,
//		D3D_DRIVER_TYPE_HARDWARE,
//		0,
//		nCreateDeviceFlags,
//		NULL,
//		0,
//		D3D11_SDK_VERSION,
//		&m_pD3dDevice,
//		&eFeatureLevel,
//		&m_pD3dContext);
//
//	if (FAILED(hr))
//	{
//		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
//		assert(NULL && L"D3D11CreateDevice Failed.");
//	}
//
//	if (eFeatureLevel != D3D_FEATURE_LEVEL_11_0)
//	{
//		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
//		return false;
//	}
//
//	UINT			n4xMsaaQuality;
//	HR(m_pD3dDevice->CheckMultisampleQualityLevels(
//		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &n4xMsaaQuality));
//
//	if (n4xMsaaQuality <= 0)
//	{
//		MessageBox(0, L"Direct3D MultisampleQualityLevel unsupported.", 0, 0);
//		return false;
//	}
//
//	RECT rc;
//	GetClientRect(g_hwnd, &rc);
//
//	DXGI_SWAP_CHAIN_DESC					sd;
//	sd.BufferDesc.Width = rc.right;
//	sd.BufferDesc.Height = rc.bottom;
//	sd.BufferDesc.RefreshRate.Numerator = 60;
//	sd.BufferDesc.RefreshRate.Denominator = 1;
//	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
//	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
//	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
//
//	if (n4xMsaaQuality > 0)
//	{
//		sd.SampleDesc.Count = 4;
//		sd.SampleDesc.Quality = n4xMsaaQuality - 1;
//	}
//	else
//	{
//		sd.SampleDesc.Count = 1;
//		sd.SampleDesc.Quality = 0;
//	}
//
//	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	sd.BufferCount = 1;
//	sd.OutputWindow = g_hwnd;
//	sd.Windowed = true;
//	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
//	sd.Flags = 0;
//
//	IDXGIDevice*			pDxgiDevice = NULL;
//	IDXGIAdapter*			pDxgiAdapter = NULL;
//	IDXGIFactory*			pDxgiFactory = NULL;
//	IDXGISwapChain*			pDxgiSwapChain = NULL;
//
//	HR(m_pD3dDevice->QueryInterface(_uuidof(IDXGIDevice), (void**)&pDxgiDevice));
//	HR(pDxgiDevice->GetParent(_uuidof(IDXGIAdapter), (void**)&pDxgiAdapter));
//	HR(pDxgiAdapter->GetParent(_uuidof(IDXGIFactory), (void**)&pDxgiFactory));
//	HR(pDxgiFactory->CreateSwapChain(m_pD3dDevice, &sd, &pDxgiSwapChain));
//
//	SAFE_RELEASE(pDxgiDevice);
//	SAFE_RELEASE(pDxgiAdapter);
//	SAFE_RELEASE(pDxgiFactory);
//
//	ID3D11RenderTargetView*		pRenderTargetView = NULL;
//	ID3D11Texture2D*			pBackBuffer = NULL;
//	pDxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
//		reinterpret_cast<void**>(&pBackBuffer));
//	m_pD3dDevice->CreateRenderTargetView(pBackBuffer, 0,
//		&pRenderTargetView);
//	SAFE_RELEASE(pBackBuffer);

	//Setting DepthStencil

	//D3D11_TEXTURE2D_DESC				depthStencilDesc;
	//depthStencilDesc.Width				= rc.right;
	//depthStencilDesc.Height				= rc.bottom;
	//depthStencilDesc.MipLevels			= 1;
	//depthStencilDesc.ArraySize			= 1;
	//depthStencilDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;

	//if (n4xMsaaQuality > 0)
	//{
	//	depthStencilDesc.SampleDesc.Count	= 4;
	//	depthStencilDesc.SampleDesc.Quality = n4xMsaaQuality;
	//}
	//else
	//{
	//	depthStencilDesc.SampleDesc.Count	= 1;
	//	depthStencilDesc.SampleDesc.Quality = 0;
	//}

	//depthStencilDesc.Usage				= D3D11_USAGE_DEFAULT;
	//depthStencilDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	//depthStencilDesc.CPUAccessFlags		= 0;
	//depthStencilDesc.MiscFlags			= 0;

	//ID3D11Texture2D*			pDepthStencilBuffer = NULL;
	//ID3D11DepthStencilView*		pDepthStencilView	= NULL;

	//HR(m_pD3dDevice->CreateTexture2D(
	//	&depthStencilDesc,
	//	0,
	//	&pDepthStencilBuffer));
	//HR(m_pD3dDevice->CreateDepthStencilView(
	//	pDepthStencilBuffer,
	//	0,
	//	&pDepthStencilView));

	//m_pD3dContext->OMSetRenderTargets(
	//	1, &pRenderTargetView, pDepthStencilView);


	//Setting Viewport

	//D3D11_VIEWPORT vp;
	//vp.TopLeftX = 0.0f;
	//vp.TopLeftY = 0.0f;
	//vp.Width = static_cast<float>(rc.right);
	//vp.Height = static_cast<float>(rc.bottom);
	//vp.MinDepth = 0.0f;
	//vp.MaxDepth = 1.0f;

}

ID3D11Device* cDeviceManager::GetDevice()
{
	return m_pD3dDevice;
}

ID3D11DeviceContext* cDeviceManager::GetContext()
{
	return m_pD3dContext;
}

void cDeviceManager::Destroy()
{
	SAFE_RELEASE(m_pD3dDevice);
	SAFE_RELEASE(m_pD3dContext);
}

