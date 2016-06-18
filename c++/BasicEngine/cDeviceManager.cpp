#include "stdafx.h"
#include "cDeviceManager.h"




cDeviceManager::cDeviceManager(void)
{

}

cDeviceManager::~cDeviceManager(void)
{

}

bool cDeviceManager::CreateDevice()
{
	UINT nCreateDeviceFlags = 0;

#if defined(DEBUG) || defined(_DEBUG)
	nCreateDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif 

	D3D_FEATURE_LEVEL			eFeatureLevel;
	ID3D11Device*				pD3dDevice;
	ID3D11DeviceContext*		pD3dContext;
	HRESULT						hr = D3D11CreateDevice(
		NULL,
		D3D_DRIVER_TYPE_HARDWARE,
		0,
		nCreateDeviceFlags,
		NULL,
		0,
		D3D11_SDK_VERSION,
		&pD3dDevice,
		&eFeatureLevel,
		&pD3dContext);

	if (FAILED(hr))
	{
		MessageBox(0, L"D3D11CreateDevice Failed.", 0, 0);
		assert(NULL && L"D3D11CreateDevice Failed.");
	}

	if (eFeatureLevel != D3D_FEATURE_LEVEL_11_0)
	{
		MessageBox(0, L"Direct3D Feature Level 11 unsupported.", 0, 0);
		return false;
	}

	UINT			n4xMsaaQuality;
	HR(pD3dDevice->CheckMultisampleQualityLevels(
		DXGI_FORMAT_R8G8B8A8_UNORM, 4, &n4xMsaaQuality));

	if (n4xMsaaQuality <= 0)
	{
		MessageBox(0, L"Direct3D MultisampleQualityLevel unsupported.", 0, 0);
		return false;
	}

	RECT rc;
	GetClientRect(g_hwnd, &rc);

	DXGI_SWAP_CHAIN_DESC					sd;
	sd.BufferDesc.Width = rc.right;
	sd.BufferDesc.Height = rc.bottom;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	sd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	if (n4xMsaaQuality > 0)
	{
		sd.SampleDesc.Count = 4;
		sd.SampleDesc.Quality = n4xMsaaQuality - 1;
	}
	else
	{
		sd.SampleDesc.Count = 1;
		sd.SampleDesc.Quality = 0;
	}

	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	sd.BufferCount = 1;
	sd.OutputWindow = g_hwnd;
	sd.Windowed = true;
	sd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	sd.Flags = 0;

	IDXGIDevice*			pDxgiDevice = NULL;
	IDXGIAdapter*			pDxgiAdapter = NULL;
	IDXGIFactory*			pDxgiFactory = NULL;
	IDXGISwapChain*			pDxgiSwapChain = NULL;

	HR(pD3dDevice->QueryInterface(_uuidof(IDXGIDevice), (void**)&pDxgiDevice));
	HR(pDxgiDevice->GetParent(_uuidof(IDXGIAdapter), (void**)&pDxgiAdapter));
	HR(pDxgiAdapter->GetParent(_uuidof(IDXGIFactory), (void**)&pDxgiFactory));
	HR(pDxgiFactory->CreateSwapChain(pD3dDevice, &sd, &pDxgiSwapChain));

	SAFE_RELEASE(pDxgiDevice);
	SAFE_RELEASE(pDxgiAdapter);
	SAFE_RELEASE(pDxgiFactory);

	ID3D11RenderTargetView* pRenderTargetView;
	ID3D11Texture2D* pBackBuffer;
	pDxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D),
		reinterpret_cast<void**>(&pBackBuffer));
	pD3dDevice->CreateRenderTargetView(pBackBuffer, 0,
		&pRenderTargetView);
	SAFE_RELEASE(pBackBuffer);

	D3D11_TEXTURE2D_DESC				depthStencilDesc;
	depthStencilDesc.Width				= rc.right;
	depthStencilDesc.Height				= rc.bottom;
	depthStencilDesc.MipLevels			= 1;
	depthStencilDesc.ArraySize			= 1;
	depthStencilDesc.Format				= DXGI_FORMAT_D24_UNORM_S8_UINT;

	if (n4xMsaaQuality > 0)
	{
		depthStencilDesc.SampleDesc.Count	= 4;
		depthStencilDesc.SampleDesc.Quality = n4xMsaaQuality;
	}
	else
	{
		depthStencilDesc.SampleDesc.Count	= 1;
		depthStencilDesc.SampleDesc.Quality = 0;
	}

	depthStencilDesc.Usage				= D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags			= D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags		= 0;
	depthStencilDesc.MiscFlags			= 0;

	ID3D11Texture2D*			pDepthStencilBuffer = NULL;
	ID3D11DepthStencilView*		pDepthStencilView	= NULL;

	HR(pD3dDevice->CreateTexture2D(
		&depthStencilDesc,
		0,
		&pDepthStencilBuffer));
	HR(pD3dDevice->CreateDepthStencilView(
		pDepthStencilBuffer,
		0,
		&pDepthStencilView));

	pD3dContext->OMSetRenderTargets(
		1, &pRenderTargetView, pDepthStencilView);

	D3D11_VIEWPORT vp;
	vp.TopLeftX = 0.0f;
	vp.TopLeftY = 0.0f;
	vp.Width = static_cast<float>(rc.right);
	vp.Height = static_cast<float>(rc.bottom);
	vp.MinDepth = 0.0f;
	vp.MaxDepth = 1.0f;

}

