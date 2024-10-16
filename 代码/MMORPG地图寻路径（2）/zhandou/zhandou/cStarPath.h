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
	stNode* pParent;	//当前节点的路径的前一个结点
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

	void MoveTo(cRoleObject* pRole,tagPOINT pt);	//移动
	void InitObstruct();	//初始化阻挡点信息

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

	cMapEdit m_mapEdit;	//加载地图
	int m_PathLattice[Max_Row][Max_Row];	//地图阻挡点信息

	map<cRoleObject*,PathList> m_mapRolePathList;//所以角色路径map
};

