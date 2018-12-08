#include "stdafx.h"
#include "cLobby.h"
#include "cGame.h"


cLobby::cLobby()
{
}


cLobby::~cLobby()
{
}

void cLobby::Init()
{
	m_szLobby = "게임 시작 Y/N";
}

void cLobby::Update()
{
	//해당 업데이트 함수를 호출했을때 'Y'의 키보드가 눌러진 상태라면 GetKeyState('Y')는 0x8000를 포함하는 숫자이다.
	if (GetKeyState('Y') & 0x8000)
	{
		//로비씬을 게임씬으로 바꿔달라고 콜랙을 보냄(m_pDelegate == cMain)
		m_pDelegate->ChangeScene(new cGame);
	}

	//해당 업데이트 함수를 호출했을때 'N'의 키보드가 눌러진 상태라면 GetKeyState('Y')는 0x8000를 포함하는 숫자이다.
	if (GetKeyState('N') & 0x8000)
	{
		g_isQuit = true;
	}
}

void cLobby::Render()
{
	ScreenPrint(0, 0, &m_szLobby[0]);
	//cout << m_szLobby << endl;
}
