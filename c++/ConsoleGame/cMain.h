#pragma once

#include "iScene.h"

struct cMain : public iSceneDelegate
{
private:
	//게임 씬 클래스 변수
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

