#include "StdAfx.h"
#include "cStarPath.h"


cStarPath::cStarPath(wchar_t* path)
{
	if (path != NULL)
	{
		wchar_t curPath[MAX_PATH];
		GetCurrentDirectory(MAX_PATH,curPath);
		wsprintf(curPath,L"%s\\..\\debug\\%s",curPath,path);

		m_mapEdit.LoadFileFromPath(curPath);
		//初始化阻挡点
		InitObstruct();
		AddTimer(1,40);
	}
}


cStarPath::~cStarPath(void)
{
}

void cStarPath::MoveTo(cRoleObject* pRole,tagPOINT ptDes)
{
	if (pRole == NULL)
	{
		return;
	}

	
	//找到了起来和终点
	int rowStart,rankStart,rowEnd,rankEnd;
	
	//获得起的和终点的行和列
	rowStart = m_mapEdit.GetRow(pRole->m_pPictureInfo->ptPos);
	rankStart = m_mapEdit.GetRank(pRole->m_pPictureInfo->ptPos);
	rowEnd = m_mapEdit.GetRow(ptDes);
	rankEnd = m_mapEdit.GetRank(ptDes);

	if (m_PathLattice[rowEnd][rankStart] == 1)
	{
		return;
	}

	if (pRole->m_roleStatus == Status_Move)
	{
		//把原来的的路径要删除
		DelPathFromMap(pRole);
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
		ClearList(listStart);
		ClearList(listEnd);
		delete nodeEnd;
		return ;
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
		ClearList(listStart);
		ClearList(listEnd);
		delete nodeEnd;
		return ;
	}
	PathList path;

	while(pNodeFind->pParent)
	{
		//m_PathLattice[pNodeFind->row][pNodeFind->rank] = 2;
		tagPOINT pt;
		pt.x = pNodeFind->rank*LatticeLen+LatticeLen/2;
		pt.y = pNodeFind->row*LatticeLen+LatticeLen/2;
		stPathNode stPath;
		stPath.pt = pt;
		path.push_front(stPath);
		pNodeFind = pNodeFind->pParent;
	}

	tagPOINT pt;
	pt.x = pNodeFind->rank*LatticeLen+LatticeLen/2;
	pt.y = pNodeFind->row*LatticeLen+LatticeLen/2;
	stPathNode stPath;
	stPath.pt = pt;
	path.push_front(stPath);
	//把最一个点直接变成目标点
	path.rbegin()->pt = ptDes;

	if (path.size()>1)
	{
		path.pop_front();
	}

	m_mapRolePathList.insert(make_pair(pRole,path));
	pRole->m_roleStatus =Status_Move;

	ClearList(listStart);
	ClearList(listEnd);

	delete nodeEnd;
}

void cStarPath::InitObstruct()
{
	ZeroMemory(m_PathLattice,Max_Row*Max_Rank);
	for (list<tagPOINT>::iterator it = m_mapEdit.m_listObstruct.begin();
		it != m_mapEdit.m_listObstruct.end();it++)
	{
		//算出是第几行第几列
		int row = m_mapEdit.GetRow(*it);
		int rank = m_mapEdit.GetRank(*it);

		m_PathLattice[row][rank] = 1;
	}
}



void cStarPath::ClearList(list<stNode*> nodeList)
{
	for (list<stNode*>::iterator it = nodeList.begin();
		it != nodeList.end();it++)
	{
		delete *it;
	}
}

int cStarPath::Distance(int row1,int rank1,int row2,int rank2)
{
	int x1 = rank1*LatticeLen+LatticeLen/2;
	int y1 = row1*LatticeLen+LatticeLen/2;
	int x2 = rank2*LatticeLen+LatticeLen/2;
	int y2 = row2*LatticeLen+LatticeLen/2;

	//算出两个格子的中心坐标
	return (int)sqrt((float)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
}

//这个结点是否已经在列表中
bool cStarPath::IsNodeInList(stNode* pNode,list<stNode*> nodeList)
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

stNode* cStarPath::GetNearestNode(list<stNode*> nodeList)
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

void cStarPath::GetNearNodeList(stNode* pNode,list<stNode*>& listNear,
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
			if (rowTemp < 0 || rankTemp < 0 || 
				rowTemp >= m_mapEdit.m_mapRow || rankTemp>=m_mapEdit.m_mapRank)
			{
				continue;
			}
			//阻挡点
			if (m_PathLattice[rowTemp][rankTemp] == 1)
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

void cStarPath::EraseFromList(stNode* pNode,list<stNode*>& nodeList)
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

int cStarPath::OnTimer(int id,int iParam,string str)
{
	for (map<cRoleObject*,PathList>::iterator it = m_mapRolePathList.begin();
		it != m_mapRolePathList.end();)
	{
		cRoleObject* pRole = it->first;
		PathList* pathList = &(it->second);
		if (pathList->size() == 0)
		{
			it = m_mapRolePathList.erase(it);
			pRole->m_roleStatus = Status_Stop;
			continue;
		}
		tagPOINT ptDes = (*(pathList->begin())).pt;
		tagPOINT ptCur = pRole->m_pPictureInfo->ptPos;
		if (sqrt((float)((ptDes.x-ptCur.x)*(ptDes.x-ptCur.x)+
			(ptDes.y-ptCur.y)*(ptDes.y-ptCur.y)))<10)
		{
			pRole->m_pPictureInfo->ptPos.x = ptDes.x;
			pRole->m_pPictureInfo->ptPos.y = ptDes.y;
			pathList->pop_front();
		}
		else
		{
			MoveOneStep(pRole,ptDes);
		}
		it++;
	}
	return 1;
}

void cStarPath::MoveOneStep(cRoleObject* pRole,tagPOINT ptDes)
{
	tagPOINT* ptRes = &(pRole->m_pPictureInfo->ptPos);
	if (ptDes.x == ptRes->x)
	{
		if (ptDes.y > ptRes->y)
		{
			ptRes->y += Step_Value;
		}
		else
		{
			ptRes->y -= Step_Value;
		}
		return;
	}

	float temp = (float)abs(ptDes.y-ptRes->y)/abs(ptDes.x-ptRes->x);
	//lenY = lenX*temp  lenX*lenX + lenY*lenY == step_value*step_value
	float lenX = sqrt((float)(Step_Value*Step_Value)/(1+temp*temp));
	float lenY = lenX*temp;

	if (ptRes->y > ptDes.y)
	{
		ptRes->y -= lenY;
	}
	else
	{
		ptRes->y += lenY;
	}
	if (ptRes->x>ptDes.x)
	{
		ptRes->x -= lenX;
	}
	else
	{
		ptRes->x += lenX;
	}
}

void cStarPath::DelPathFromMap(cRoleObject* pRole)
{
	for (map<cRoleObject*,PathList>::iterator it = m_mapRolePathList.begin();
		it != m_mapRolePathList.end();it++)
	{
		cRoleObject* pRoleTemp = it->first;
		if (pRoleTemp == pRole)
		{
			m_mapRolePathList.erase(it);
			break;
		}
	}
}