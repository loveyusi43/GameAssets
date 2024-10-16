#pragma once
#include "cMyTimer.h"

//����������
#define Max_Row 7
#define Max_Rank 9

enum Timer_Type
{
	Timer_Draw,	//���ƶ�ʱ��
	Timer_ClearJellyStep1,	//��һ������ʾ�ƺ��״̬
	Timer_ClearJellyStep2,	//ըʱ״
};

//�������ͣ���������)
enum JellyType
{
	JellyType1,
	JellyType2,
	JellyType3,
	JellyType4,
	JellyType5,
	JellyType6,
	JellyType7,
	JellyTypeNum,
	JellyDes,	//Ҫ�ݻٵ�Ŀ��
	JellyNULL,
	ClearPicRow,	//�����̻�
	ClearPicRank	//�����̻�
};

class cJelly:public cMyTimer
{
public:
	cJelly(HWND hWnd);
	~cJelly(void);

	//���ƺ���
	void Draw();
	//���ƹ���
	void DrawJelly(HDC memDc);

	//��ʱ���ص�����
	virtual int OnTimer(int id,int iParam,string str);

	//��ʼ����Ϸ����
	void InitGame();
	//����¼���Ӧ
	void OnLButtonDown(LPARAM lParam);

	void OnLButtonUp(LPARAM lParam);

	void OnMouseMove(LPARAM lParam);

	//�жϵ�ǰ��������У����Ƿ��ڱ��϶����ӵ��Ա�
	bool IsNearDragJelly(int row,int rank);
	//�ж��Ƿ�����������
	bool CheckClear(JellyType jellyInfo[][Max_Rank]);
	//����
	void Clear();
	//������������Ϊը��
	void ClearJellyProc();
	//���¹���
	void UpdateJelly();

	//���ʣ��Ҫը�ٵ�Ŀ�����
	int GetDesNum();
private:
	HWND m_hWnd; //���ھ��

	JellyType m_JellyArray[Max_Row][Max_Rank];	//����ÿ��������ʲô
	bool m_bIsButtonDown;	//����Ƿ��ǰ���״̬
	tagPOINT m_posCurDragJelly;	//��ǰ�϶��Ĺ���������x- ��  y-��

	//��������Ϣ
	tagPOINT m_firstWillClearJelly;	//��һ��Ҫ�����Ĺ�����λ��x- ��  y-��
	bool m_bRow;	//�Ƿ��Ǻ�������
	int m_iClearNum;	//һ��Ҫ�������ٸ�����

	//�ؿ���Ϣ
	int m_curLevel;	//��ǰ��
	int m_curLeaveStep;	//ʣ�ಽ��
};

