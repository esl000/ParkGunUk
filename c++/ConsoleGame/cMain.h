#pragma once

#include "iScene.h"

struct cMain : public iSceneDelegate
{
private:
	//���� �� Ŭ���� ����
	iScene* m_pScene;

public:
	cMain();
	~cMain();

	void Init();
	void Update();
	void Render();

	virtual void OnSceneFinished();
	virtual void ChangeScene(iScene* scene);

};

