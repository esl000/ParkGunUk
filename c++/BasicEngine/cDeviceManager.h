#pragma once

#define g_pD3DDevice cDeviceManager::GetInstance()->GetDevice()
#define g_pD3DContext cDeviceManager::GetInstance()->GetContext()

class cDeviceManager
{
private:

	bool m_vsync_enabled;
	int m_videoCardMemory;
	char m_videoCardDescription[128];
	IDXGISwapChain* m_swapChain;
	ID3D11Device* m_device;
	ID3D11DeviceContext* m_deviceContext;
	ID3D11RenderTargetView* m_renderTargetView;
	ID3D11Texture2D* m_depthStencilBuffer;
	ID3D11DepthStencilState* m_depthStencilState;
	ID3D11DepthStencilView* m_depthStencilView;
	ID3D11RasterizerState* m_rasterState;
	D3DXMATRIX m_projectionMatrix;
	D3DXMATRIX m_worldMatrix;
	D3DXMATRIX m_orthoMatrix;

private:
	SINGLETONE(cDeviceManager)

public:
	bool CreateDevice();
	ID3D11Device* GetDevice();
	ID3D11DeviceContext* GetContext();
	void Destroy();
};

