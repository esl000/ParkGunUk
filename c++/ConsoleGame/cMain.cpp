#include "stdafx.h"
#include "cMain.h"
#include "cLobby.h"


//���̳ʸ� �ʱ�ȭ m_pScene
cMain::cMain()
	:m_pScene(new cLobby)
{
}


cMain::~cMain()
{
	delete m_pScene;
}

void cMain::Init()
{
	if (g_isQuit)
		return;
	//�� ��������Ʈ�� ���� �� �ν��Ͻ��� ����
	m_pScene->SetDelegate(this);
	//�� �ʱ�ȭ
	m_pScene->Init();
}

void cMain::Update()
{
	if (g_isQuit)
		return;
	//�� ������Ʈ
	m_pScene->Update();
}

void cMain::Render()
{
	if (g_isQuit)
		return;
	//�� ������
	m_pScene->Render();
}

void cMain::OnSceneFinished()
{

}

//���ٲ�����
void cMain::ChangeScene(iScene* scene)
{
	//���� �� �޸� ����
	delete m_pScene;
	//���ڰ����� �������� ������ ������ ����
	m_pScene = scene;
	//�� �ʱ�ȭ
	m_pScene->Init();
}
