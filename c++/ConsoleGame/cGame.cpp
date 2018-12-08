#include "stdafx.h"
#include "cGame.h"
#include "cStage.h"


cGame::cGame()
	: m_pLevel(new cStage)
{
}


cGame::~cGame()
{
	delete m_pLevel;
}

void cGame::Init()
{
	m_pLevel->SetDelegate(this);
	m_pLevel->Init();
}

void cGame::Update()
{
	m_pLevel->Update();
}

void cGame::Render()
{
	m_pLevel->Render();
}

void cGame::OnLevelFinished()
{
	//���� �ε���
	DWORD dwIndex;
	//���� ������� ������ �ε��� ������
	dwIndex = m_pLevel->GetLevelIndex();
	//���� ���� �޸� ����
	delete m_pLevel;
	//���ο� ���� �����Ҵ�
	m_pLevel = new cStage;
	//���ο� ���� ��������Ʈ�� ���� �� �ν��Ͻ��� ����
	m_pLevel->SetDelegate(this);
	//���ο� ���� �ε����� �Ʊ��� ���� �ε��� + 1�� �Ͽ� ����
	m_pLevel->SetLevelIndex(dwIndex + 1);
	//���� �ʱ�ȭ
	m_pLevel->Init();
}
