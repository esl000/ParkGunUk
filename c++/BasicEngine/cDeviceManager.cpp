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


	// vsync(��������ȭ) ������ �����մϴ�.
	m_vsync_enabled = vsync;

	// DirectX �׷��� �������̽� ���丮�� ����ϴ�.
	result = CreateDXGIFactory(__uuidof(IDXGIFactory), (void**)&factory);
	if (FAILED(result))
	{
		return false;
	}

	// ���丮 ��ü�� ����Ͽ� ù��° �׷��� ī�� �������̽��� ���� �ƴ��͸� ����ϴ�.
	result = factory->EnumAdapters(0, &adapter);
	if (FAILED(result))
	{
		return false;
	}

	// ���(�����)�� ���� ù��° �ƴ��͸� �����մϴ�.
	result = adapter->EnumOutputs(0, &adapterOutput);
	if (FAILED(result))
	{
		return false;
	}

	// DXGI_FORMAT_R8G8B8A8_UNORM ����� ��� ���÷��� ���˿� �´� ����� ������ ���մϴ�.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, NULL);
	if (FAILED(result))
	{
		return false;
	}

	// ������ ��� ����Ϳ� �׷���ī�� ������ ������ ����Ʈ�� �����մϴ�.
	displayModeList = new DXGI_MODE_DESC[numModes];
	if (!displayModeList)
	{
		return false;
	}

	// ���÷��� ��忡 ���� ����Ʈ ������ ä���ֽ��ϴ�.
	result = adapterOutput->GetDisplayModeList(DXGI_FORMAT_R8G8B8A8_UNORM, DXGI_ENUM_MODES_INTERLACED, &numModes, displayModeList);
	if (FAILED(result))
	{
		return false;
	}

	// ���� ��� ���÷��� ��忡 ���� ȭ�� �ʺ�/���̿� �´� ���÷��� ��带 ã���ϴ�.
	// ������ ���� ã���� ������� ���ΰ�ħ ������ �и�� ���� ���� �����մϴ�.
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

	// �ƴ���(�׷���ī��)�� description�� �����ɴϴ�.
	result = adapter->GetDesc(&adapterDesc);
	if (FAILED(result))
	{
		return false;
	}

	// ���� �׷���ī���� �޸� �뷮�� �ް�����Ʈ ������ �����մϴ�.
	m_videoCardMemory = (int)(adapterDesc.DedicatedVideoMemory / 1024 / 1024);

	// �׷���ī���� �̸��� char�� ���ڿ� �迭�� �ٲ� �� �����մϴ�.
	error = wcstombs_s(&stringLength, m_videoCardDescription, 128, adapterDesc.Description, 128);
	if (error != 0)
	{
		return false;
	}

	// ���÷��� ��� ����Ʈ�� �Ҵ��� �����մϴ�.
	delete[] displayModeList;
	displayModeList = 0;

	// ��� �ƴ��͸� �Ҵ� �����մϴ�.
	adapterOutput->Release();
	adapterOutput = 0;

	// �ƴ��͸� �Ҵ� �����մϴ�.
	adapter->Release();
	adapter = 0;

	// ���丮 ��ü�� �Ҵ� �����մϴ�.
	factory->Release();
	factory = 0;

	// ���� ü�� description�� �ʱ�ȭ�մϴ�.
	ZeroMemory(&swapChainDesc, sizeof(swapChainDesc));

	// �ϳ��� ����۸��� ����ϵ��� �մϴ�.
	swapChainDesc.BufferCount = 1;

	// ������� �ʺ�� ���̸� �����մϴ�.
	swapChainDesc.BufferDesc.Width = screenWidth;
	swapChainDesc.BufferDesc.Height = screenHeight;

	// ����۷� �Ϲ����� 32bit�� �����̽��� �����մϴ�.
	swapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;

	// ������� ���ΰ�ħ ������ �����մϴ�.
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

	// ������� �뵵�� �����մϴ�.
	swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;

	// �������� �̷���� �������� �ڵ��� �����մϴ�.
	swapChainDesc.OutputWindow = hwnd;

	// ��Ƽ���ø��� ���ϴ�.
	swapChainDesc.SampleDesc.Count = 1;
	swapChainDesc.SampleDesc.Quality = 0;

	// ������ ��� �Ǵ� Ǯ��ũ�� ��带 �����մϴ�.
	if (fullscreen)
	{
		swapChainDesc.Windowed = false;
	}
	else
	{
		swapChainDesc.Windowed = true;
	}

	// ��ĵ������ ���İ� ��ĵ���̴��� �������� ��������(unspecified) �����մϴ�.
	swapChainDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapChainDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	// ��µ� ������ ������� ������ �������� �մϴ�.
	swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	// �߰� �ɼ� �÷��׸� ������� �ʽ��ϴ�.
	swapChainDesc.Flags = 0;

	// ���� ������ DirectX 11�� �����մϴ�.
	featureLevel = D3D_FEATURE_LEVEL_11_0;

	// ���� ü��, Direct3D ����̽�, Direct3D ����̽� ���ؽ�Ʈ�� �����մϴ�.
	result = D3D11CreateDeviceAndSwapChain(NULL, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &featureLevel, 1,
		D3D11_SDK_VERSION, &swapChainDesc, &m_swapChain, &m_device, NULL, &m_deviceContext);
	if (FAILED(result))
	{
		return false;
	}

	// ������� �����͸� �����ɴϴ�.
	result = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (LPVOID*)&backBufferPtr);
	if (FAILED(result))
	{
		return false;
	}

	// ������� �����ͷ� ����Ÿ�� �並 �����մϴ�.
	result = m_device->CreateRenderTargetView(backBufferPtr, NULL, &m_renderTargetView);
	if (FAILED(result))
	{
		return false;
	}

	// ����� �����͸� ���̻� ������� �����Ƿ� �Ҵ� �����մϴ�.
	backBufferPtr->Release();
	backBufferPtr = 0;

	// ���� ������ description�� �ʱ�ȭ�մϴ�.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// ���� ������ description�� �ۼ��մϴ�.
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

	// description�� ����Ͽ� ���� ������ �ؽ��ĸ� �����մϴ�.
	result = m_device->CreateTexture2D(&depthBufferDesc, NULL, &m_depthStencilBuffer);
	if (FAILED(result))
	{
		return false;
	}
	
	// ���ٽ� ������ description�� �ʱ�ȭ�մϴ�.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// ���ٽ� ������ description�� �ۼ��մϴ�.
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

	// ����-���ٽ� ���¸� �����մϴ�.
	result = m_device->CreateDepthStencilState(&depthStencilDesc, &m_depthStencilState);
	if (FAILED(result))
	{
		return false;
	}

	// ����-���ٽ� ���¸� �����մϴ�.
	m_deviceContext->OMSetDepthStencilState(m_depthStencilState, 1);

	// ����-���ٽ� ���� description�� �ʱ�ȭ�մϴ�.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// ����-���ٽ� ���� description�� �ۼ��մϴ�.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// ����-���ٽ� �並 �����մϴ�.
	result = m_device->CreateDepthStencilView(m_depthStencilBuffer, &depthStencilViewDesc, &m_depthStencilView);
	if (FAILED(result))
	{
		return false;
	}

	// ����Ÿ�� ��� ����-���ٽ� ���۸� ���� ��� ���������ο� ���ε��մϴ�.
	m_deviceContext->OMSetRenderTargets(1, &m_renderTargetView, m_depthStencilView);


	// � ������ ��� �׸� ������ �����ϴ� ������ȭ�� description�� �ۼ��մϴ�.
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

	// �ۼ��� description���κ��� ������ȭ�� ���¸� �����մϴ�.
	result = m_device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	if (FAILED(result))
	{
		return false;
	}

	// ������ȭ�� ���¸� �����մϴ�.
	m_deviceContext->RSSetState(m_rasterState);

	// �������� ���� ����Ʈ�� �����մϴ�.
	viewport.Width = (float)screenWidth;
	viewport.Height = (float)screenHeight;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// ����Ʈ�� �����մϴ�.
	m_deviceContext->RSSetViewports(1, &viewport);


	// ���� ����� �����մϴ�.
	fieldOfView = (float)D3DX_PI / 4.0f;
	screenAspect = (float)screenWidth / (float)screenHeight;

	// 3D �������� ���� ���� ����� �����մϴ�.
	D3DXMatrixPerspectiveFovLH(&m_projectionMatrix, fieldOfView, screenAspect, screenNear, screenDepth);


	// ���� ����� ���� ��ķ� �ʱ�ȭ�մϴ�.
	D3DXMatrixIdentity(&m_worldMatrix);



	// 2D �������� ���� ���翵 ����� �����մϴ�.
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

