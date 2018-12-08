#pragma once
#include "iLevel.h"

//레벨 추상 클래스를 상속받은 클래스
struct cStage : public iLevel
{
protected:
	//vector를 이용한 2중배열 맵
	vector<vector<ST_OBJECT*>> m_vecMap;
	//장애물들
	vector<ST_ENEMY*> m_vecEnemy;
	//플레이어
	ST_PLAYER* m_pPlayer;

public:
	cStage();
	~cStage();

	virtual void Init();
	virtual void Update();
	virtual void Render();
};

