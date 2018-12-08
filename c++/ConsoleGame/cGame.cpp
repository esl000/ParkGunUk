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
	//레벨 인덱스
	DWORD dwIndex;
	//현재 사용중인 레벨의 인덱스 가져옴
	dwIndex = m_pLevel->GetLevelIndex();
	//현재 레벨 메모리 해제
	delete m_pLevel;
	//새로운 레벨 동적할당
	m_pLevel = new cStage;
	//새로운 레벨 딜리게이트를 현재 이 인스턴스로 설정
	m_pLevel->SetDelegate(this);
	//새로운 레벨 인덱스를 아까전 레벨 인덱스 + 1을 하여 설정
	m_pLevel->SetLevelIndex(dwIndex + 1);
	//레벨 초기화
	m_pLevel->Init();
}
