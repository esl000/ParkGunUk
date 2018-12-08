#pragma once

struct iScene;

//�� ��������Ʈ�� ���� �߻�Ŭ����(���α׷��� ������ ©�� Ŭ������ �ܹ��⼺�� ���� ����)
struct iSceneDelegate
{
public:
	virtual void OnSceneFinished() = 0;
	virtual void ChangeScene(iScene* scene) = 0;
};

//�������� �߻�Ŭ����
struct iScene
{ 
protected:
	//�� ��������Ʈ(�ݹ��� ���� �ν��Ͻ� ����)
	PROPERTY(iSceneDelegate*, m_pDelegate, Delegate);
public:
	iScene();
	~iScene();

	//���� �����Լ�(�� �߻�Ŭ������ ��ӹ��� Ŭ������ �Լ��� ������ ���� ����)
	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
};

