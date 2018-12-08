#pragma once

struct iScene;

//씬 딜리게이트를 위한 추상클래스(프로그래밍 구조를 짤때 클래스의 단방향성을 위해 사용됨)
struct iSceneDelegate
{
public:
	virtual void OnSceneFinished() = 0;
	virtual void ChangeScene(iScene* scene) = 0;
};

//씬에대한 추상클래스
struct iScene
{ 
protected:
	//씬 딜리게이트(콜백을 보낼 인스턴스 저장)
	PROPERTY(iSceneDelegate*, m_pDelegate, Delegate);
public:
	iScene();
	~iScene();

	//순수 가상함수(이 추상클래스를 상속받은 클래스가 함수의 내용을 직접 제작)
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
};

