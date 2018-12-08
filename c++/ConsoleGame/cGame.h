#pragma once
#include "iScene.h"
#include "iLevel.h"

//씬 추상 클래스를 상속받은 클래스, 레벨 딜리게이트 추상 클래스를 상속받은 클래스
struct cGame : public iScene, iLevelDelegate
{
protected:
	//레벨(맵) 변수
	iLevel* m_pLevel;
public:
	cGame();
	~cGame();

	virtual void Init();
	virtual void Update();
	virtual void Render();

	virtual void OnLevelFinished();
};

