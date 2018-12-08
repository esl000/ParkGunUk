#pragma once
#include "iScene.h"
#include "iLevel.h"

//�� �߻� Ŭ������ ��ӹ��� Ŭ����, ���� ��������Ʈ �߻� Ŭ������ ��ӹ��� Ŭ����
struct cGame : public iScene, iLevelDelegate
{
protected:
	//����(��) ����
	iLevel* m_pLevel;
public:
	cGame();
	~cGame();

	virtual void Init();
	virtual void Update();
	virtual void Render();

	virtual void OnLevelFinished();
};

