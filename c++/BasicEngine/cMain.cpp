#include "stdafx.h"
#include "cMain.h"


cMain::cMain()
{
}


cMain::~cMain()
{
	cDeviceManager::GetInstance()->Destroy();
}

void cMain::Setup()
{
	cDeviceManager::GetInstance()->CreateDevice();
}

void cMain::Update()
{

}

void cMain::Render()
{
	//// ������� ������ ����ϴ�.
	//g_pD3DContext->ClearRenderTargetView(m_renderTargetView, color);
	//// ���� ���۸� ����ϴ�.
	//g_pD3DContext->ClearDepthStencilView(m_depthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

	//g_pD3DContext->->Present(1, 0);
}
