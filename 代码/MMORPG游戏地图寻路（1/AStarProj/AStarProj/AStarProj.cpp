// AStarProj.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "math.h"
#include "list"
using namespace std;
/*
1.Ч�ʸߣ���ͼ�󣬵�λ�ࣨ�赲��ࣩ
2.�ױ༭��
3.Ч����ʵ�����·�������ٿ���ȥ�ȽϺ���


a -> b
a->a1->a2->a3     b
A*�㷨 
1.�ѵ�ͼ�и��һ��һ���ĸ���
ѡ��·����F=G+H
F:����·������
G������㣬���Ų�����·�����ƶ���ָ����ĺķѣ�·�����ȣ�
H��Ԥ��ֵ��a3��b���Ԥ��ֵ��a3��b���ֱ�߾��룩

���������������������
�����б��п��ܽ�Ҫ�����ĵ�
�ر��б�������ȥ�����ĵ�

�Ȱ����ӵ������б���
ÿ���������б�����Ȩֵ��С�ķ���������Χ�ĸ��ӣ�����Χ�ĸ��ӷŵ������б���
�ٰѵ�ǰ����������б���ɾ���ӵ��ر��б���

ֱ���յ��������б��о��ҵ��ˣ���������б���û�з����ˣ�˵��û�ҵ�·��
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
	stNode* pParent;	//��ǰ�ڵ��·����ǰһ�����
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

	//����������ӵ���������
	return (int)sqrt((float)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
}

//�������Ƿ��Ѿ����б���
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
		//�Ƚ�fֵ�������ǰ�����С��tempF
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
	//��һ������Աߵ�8���㣬3*3�ĸ���ȫ�ŵ�listNear
	//�������б��л��ڹر��б��еĲ��ܷ�
	//�赲�㲻�ܷ�
	for (int i = -1;i<=1;i++)
	{
		for (int j = -1;j<= 1;j++)
		{
			if (i == 0 && j == 0)
			{
				//����������Լ�
				continue;
			}
			int rowTemp = pNode->row + i;
			int rankTemp = pNode->rank + j;
			//�Ƿ�Խ��
			if (rowTemp < 0 || rankTemp < 0 || rowTemp > 9 || rankTemp>9)
			{
				continue;
			}
			//�赲��
			if (g_PathLattice[rowTemp][rankTemp] == 1)
			{
				continue;
			}

			//�ڿ����б��в��ܷ�
			stNode node;
			node.row = rowTemp;
			node.rank = rankTemp;
			if(IsNodeInList(&node,listStart))
			{
				continue;
			}
			//�ڹر��б���
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
	//�ҵ����������յ�
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

	//�����б�͹ر��б�
	list<stNode*> listStart;
	list<stNode*> listEnd;
	listStart.push_back(nodeStart);
	//�յ��Ƿ��������б��У�����ڣ�˵���Ѿ�����·��
	//�˳�ѭ��
	stNode* pNowNode = NULL;	//��ǰ�������Ľ��
	while(!IsNodeInList(nodeEnd,listStart))
	{
		/*if (pNowNode == NULL)
		{*/
			//�ҵ�Ȩֵ��С�ĸ�����ҵ��ǰ��
			pNowNode = GetNearestNode(listStart);

			if (pNowNode == NULL)
			{
				//˵�������б���û����
				printf("·��������\n");
				break;
			}
		//}

		list<stNode*> listNear; //��ŵ�ǰ������Χ���ϼ��������б������ĸ���
		GetNearNodeList(pNowNode,listNear,listStart,listEnd,nodeEnd);

		//�ѵ�ǰ���ŵ�listEnd����
		listEnd.push_back(pNowNode);
		//�ѵ�ǰ���������б���ɾ��
		EraseFromList(pNowNode,listStart);

		//����Χ��ȫ�ŵ������б���
		for (list<stNode*>::iterator it = listNear.begin();it != listNear.end();
			it++)
		{
			listStart.push_back(*it);
		}

		
	}
	if (pNowNode == NULL)
	{
		printf("û���ҵ�·��\n");
		ClearList(listStart);
		ClearList(listEnd);
		delete nodeEnd;
		return 0;
	}
	//�ڿ����б����ҵ��յ�
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
		printf("û��·��");
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

