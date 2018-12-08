#pragma once

//���� ��������Ʈ�� ���� �߻�Ŭ����(���α׷��� ������ ©�� Ŭ������ �ܹ��⼺�� ���� ����)
struct iLevelDelegate
{
public:
	virtual void OnLevelFinished() = 0;
};

//������ ���� �߻�Ŭ����
struct iLevel
{
protected:
	//���� �ε���(���߿� �� �ε����� �ش��ϴ� TXT ������ �Ľ��Ѵ�.)
	PROPERTY(DWORD, m_dwLevelIndex, LevelIndex);
	//���� ��������Ʈ(�ݹ��� ���� �ν��Ͻ� ����)
	PROPERTY(iLevelDelegate*, m_pDelegate, Delegate);
public:
	iLevel();
	~iLevel();

	virtual void Init() = 0;
	virtual void Update() = 0;
	virtual void Render() = 0;
};

