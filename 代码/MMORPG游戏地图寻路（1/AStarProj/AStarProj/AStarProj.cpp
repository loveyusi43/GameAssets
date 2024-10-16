// AStarProj.cpp : 定义控制台应用程序的入口点。
//

#include "stdafx.h"
#include "math.h"
#include "list"
using namespace std;
/*
1.效率高，地图大，单位多（阻挡点多）
2.易编辑，
3.效果真实（最短路径）至少看上去比较合理


a -> b
a->a1->a2->a3     b
A*算法 
1.把地图切割成一个一个的格子
选择路径：F=G+H
F:就是路径长度
G：从起点，沿着产生的路径，移动到指定点的耗费（路径长度）
H：预估值，a3到b点的预估值（a3到b点的直线距离）

用两个表来保存相关数据
启动列表：有可能将要经过的点
关闭列表：不会再去遍历的点

先把起点加到启动列表中
每次在启动列表中找权值最小的方格，找他周围的格子，把周围的格子放到启动列表中
再把当前方格从启动列表中删除加到关闭列表中

直到终点在启动列表中就找到了，如果启动列表中没有方格了，说明没找到路径
*/
#define LatticeLen 10
int g_PathLattice[10][10]=
{
	{0,0,0,0,0,0,0,0,0,0},
	{0,0,0,0,1,1,1,1,0,0},
	{0,0,0,0,1,0,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0},
	{0,0,3,0,1,0,0,4,0,0},
	{0,0,0,0,1,0,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0},
	{0,0,0,0,1,0,0,0,0,0}
};

struct stNode
{
	int row;
	int rank;
	int f;
	int g;
	int h;
	stNode* pParent;	//当前节点的路径的前一个结点
};

void ClearList(list<stNode*> nodeList)
{
	for (list<stNode*>::iterator it = nodeList.begin();
		it != nodeList.end();it++)
	{
		delete *it;
	}
}

int Distance(int row1,int rank1,int row2,int rank2)
{
	int x1 = rank1*LatticeLen+LatticeLen/2;
	int y1 = row1*LatticeLen+LatticeLen/2;
	int x2 = rank2*LatticeLen+LatticeLen/2;
	int y2 = row2*LatticeLen+LatticeLen/2;

	//算出两个格子的中心坐标
	return (int)sqrt((float)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
}

//这个结点是否已经在列表中
bool IsNodeInList(stNode* pNode,list<stNode*> nodeList)
{
	for (list<stNode*>::iterator it = nodeList.begin();
		it != nodeList.end();it++)
	{
		if (pNode->row == (*it)->row && pNode->rank == (*it)->rank)
		{
			return true;
		}
	}
	return false;
}

stNode* GetNearestNode(list<stNode*> nodeList)
{
	stNode* pNode = NULL;
	int tempF = 1000000;
	for (list<stNode*>::iterator it = nodeList.begin();it != nodeList.end();
		it++)
	{
		//比较f值，如果当前这个点小于tempF
		if ((*it)->f < tempF)
		{
			pNode = (*it);
			tempF = (*it)->f;
		}
	}
	return pNode;
}

void GetNearNodeList(stNode* pNode,list<stNode*>& listNear,
	list<stNode*> listStart,list<stNode*> listEnd,stNode* pEndNode)
{
	//把一个结点旁边的8个点，3*3的格子全放到listNear
	//在启动列表中或在关闭列表中的不能放
	//阻挡点不能放
	for (int i = -1;i<=1;i++)
	{
		for (int j = -1;j<= 1;j++)
		{
			if (i == 0 && j == 0)
			{
				//这个格子是自己
				continue;
			}
			int rowTemp = pNode->row + i;
			int rankTemp = pNode->rank + j;
			//是否越界
			if (rowTemp < 0 || rankTemp < 0 || rowTemp > 9 || rankTemp>9)
			{
				continue;
			}
			//阻挡点
			if (g_PathLattice[rowTemp][rankTemp] == 1)
			{
				continue;
			}

			//在开启列表中不能放
			stNode node;
			node.row = rowTemp;
			node.rank = rankTemp;
			if(IsNodeInList(&node,listStart))
			{
				continue;
			}
			//在关闭列表中
			if (IsNodeInList(&node,listEnd))
			{
				continue;
			}

			stNode* pNearNode = new stNode;
			pNearNode->g = pNode->g+Distance(pNode->row,pNode->rank,
				rowTemp,rankTemp);
			pNearNode->h = Distance(rowTemp,rankTemp,pEndNode->row
				,pEndNode->rank);
			pNearNode->f = pNearNode->g+pNearNode->h;
			pNearNode->row = rowTemp;
			pNearNode->rank = rankTemp;
			pNearNode->pParent = pNode;
			listNear.push_back(pNearNode);
		}
	}
}

void EraseFromList(stNode* pNode,list<stNode*>& nodeList)
{
	for (list<stNode*>::iterator it = nodeList.begin();it != nodeList.end();
		it ++)
	{
		if (pNode->row == (*it)->row && pNode->rank == (*it)->rank)
		{
			nodeList.erase(it);
			return;
		}
	}
}

int _tmain(int argc, _TCHAR* argv[])
{
	//找到了起来和终点
	int rowStart,rankStart,rowEnd,rankEnd;
	for (int i =0;i<10;i++)
	{
		for (int j =0;j<10;j++)
		{
			if (g_PathLattice[i][j] == 3)
			{
				rowStart = i;
				rankStart = j;
			}
			else if (g_PathLattice[i][j] == 4)
			{
				rowEnd = i;
				rankEnd = j;
			}
		}
	}
	stNode* nodeStart = new stNode;
	nodeStart->row = rowStart;
	nodeStart->rank = rankStart;
	nodeStart->g = 0;
	nodeStart->h = Distance(rowStart,rankStart,rowEnd,rankEnd);
	nodeStart->f = nodeStart->h;
	nodeStart->pParent = NULL;

	stNode* nodeEnd = new stNode;
	nodeEnd->row = rowEnd;
	nodeEnd->rank = rankEnd;

	//启动列表和关闭列表
	list<stNode*> listStart;
	list<stNode*> listEnd;
	listStart.push_back(nodeStart);
	//终点是否有启动列表中，如果在，说明已经找至路径
	//退出循环
	stNode* pNowNode = NULL;	//当前遍历到的结点
	while(!IsNodeInList(nodeEnd,listStart))
	{
		/*if (pNowNode == NULL)
		{*/
			//找到权值最小的格子作业当前点
			pNowNode = GetNearestNode(listStart);

			if (pNowNode == NULL)
			{
				//说明启动列表中没点啦
				printf("路径不存在\n");
				break;
			}
		//}

		list<stNode*> listNear; //存放当前格子周围符合加入启动列表条件的格子
		GetNearNodeList(pNowNode,listNear,listStart,listEnd,nodeEnd);

		//把当前结点放到listEnd里面
		listEnd.push_back(pNowNode);
		//把当前结点从启动列表中删除
		EraseFromList(pNowNode,listStart);

		//把周围点全放到开启列表中
		for (list<stNode*>::iterator it = listNear.begin();it != listNear.end();
			it++)
		{
			listStart.push_back(*it);
		}

		
	}
	if (pNowNode == NULL)
	{
		printf("没有找到路径\n");
		ClearList(listStart);
		ClearList(listEnd);
		delete nodeEnd;
		return 0;
	}
	//在开启列表中找到终点
	stNode* pNodeFind = NULL;
	for (list<stNode*>::iterator it = listStart.begin();it != listStart.end();
		it++)
	{
		if ((*it)->row == nodeEnd->row &&
			(*it)->rank == nodeEnd->rank)
		{
			pNodeFind = (*it);
			break;
		}
	}
	if (pNodeFind == NULL)
	{
		printf("没有路径");
		ClearList(listStart);
		ClearList(listEnd);
		delete nodeEnd;
		return 0;
	}

	while(pNodeFind)
	{
		g_PathLattice[pNodeFind->row][pNodeFind->rank] = 2;
		pNodeFind = pNodeFind->pParent;
	}

	for (int i =0;i<10;i++)
	{
		for (int j = 0;j<10;j++)
		{
			if (g_PathLattice[i][j] == 0)
			{
				printf("^ ");
			}
			else if (g_PathLattice[i][j] == 1)
			{
				printf("* ");
			}
			else if (g_PathLattice[i][j] == 2)
			{
				printf("# ");
			}
		}
		printf("\n");
	}
	ClearList(listStart);
	ClearList(listEnd);

	delete nodeEnd;
	return 0;
}

