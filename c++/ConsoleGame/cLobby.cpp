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
	m_szLobby = "���� ���� Y/N";
}

void cLobby::Update()
{
	//�ش� ������Ʈ �Լ��� ȣ�������� 'Y'�� Ű���尡 ������ ���¶�� GetKeyState('Y')�� 0x8000�� �����ϴ� �����̴�.
	if (GetKeyState('Y') & 0x8000)
	{
		//�κ���� ���Ӿ����� �ٲ�޶�� �ݷ��� ����(m_pDelegate == cMain)
		m_pDelegate->ChangeScene(new cGame);
	}

	//�ش� ������Ʈ �Լ��� ȣ�������� 'N'�� Ű���尡 ������ ���¶�� GetKeyState('Y')�� 0x8000�� �����ϴ� �����̴�.
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
