#pragma once
#include "iLevel.h"

//���� �߻� Ŭ������ ��ӹ��� Ŭ����
struct cStage : public iLevel
{
protected:
	//vector�� �̿��� 2�߹迭 ��
	vector<vector<ST_OBJECT*>> m_vecMap;
	//��ֹ���
	vector<ST_ENEMY*> m_vecEnemy;
	//�÷��̾�
	ST_PLAYER* m_pPlayer;

public:
	cStage();
	~cStage();

	virtual void Init();
	virtual void Update();
	virtual void Render();
};

