#pragma once
#include "cRoleObject.h"
#include "cMapEdit.h"
#include "map"
using namespace std;

#define Max_Row		100

#define Max_Rank	100
#define Step_Value  10

struct stNode
{
	int row;
	int rank;
	int f;
	int g;
	int h;
	stNode* pParent;	//��ǰ�ڵ��·����ǰһ�����
};

struct stPathNode
{
	tagPOINT pt;
	bool bDel;
	stPathNode()
	{
		bDel = false;
	}
};

typedef list<stPathNode> PathList;

class cStarPath:public cMyTimer 
{
public:
	cStarPath(wchar_t* path);
	~cStarPath(void);

	void MoveTo(cRoleObject* pRole,tagPOINT pt);	//�ƶ�
	void InitObstruct();	//��ʼ���赲����Ϣ

	void ClearList(list<stNode*> nodeList);
	int Distance(int row1,int rank1,int row2,int rank2);
	bool IsNodeInList(stNode* pNode,list<stNode*> nodeList);
	stNode* GetNearestNode(list<stNode*> nodeList);
	void GetNearNodeList(stNode* pNode,list<stNode*>& listNear,
		list<stNode*> listStart,list<stNode*> listEnd,stNode* pEndNode);
	void EraseFromList(stNode* pNode,list<stNode*>& nodeList);

	virtual int OnTimer(int id,int iParam,string str);
	void MoveOneStep(cRoleObject* pRold,tagPOINT ptDes);
	void DelPathFromMap(cRoleObject* pRole);

	cMapEdit m_mapEdit;	//���ص�ͼ
	int m_PathLattice[Max_Row][Max_Row];	//��ͼ�赲����Ϣ

	map<cRoleObject*,PathList> m_mapRolePathList;//���Խ�ɫ·��map
};

