#include "stdafx.h"
#include "cMain.h"
#include "cLobby.h"


//바이너리 초기화 m_pScene
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
	//씬 딜리게이트를 현재 이 인스턴스로 설정
	m_pScene->SetDelegate(this);
	//씬 초기화
	m_pScene->Init();
}

void cMain::Update()
{
	if (g_isQuit)
		return;
	//씬 업데이트
	m_pScene->Update();
}

void cMain::Render()
{
	if (g_isQuit)
		return;
	//씬 렌더링
	m_pScene->Render();
}

void cMain::OnSceneFinished()
{

}

//씬바꼇을때
void cMain::ChangeScene(iScene* scene)
{
	//현재 신 메모리 해제
	delete m_pScene;
	//인자값으로 받은씬을 메인의 씬으로 설정
	m_pScene = scene;
	//씬 초기화
	m_pScene->Init();
}
