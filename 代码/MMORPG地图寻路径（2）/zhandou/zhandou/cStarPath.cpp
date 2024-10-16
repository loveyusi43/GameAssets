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
		//��ʼ���赲��
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

	
	//�ҵ����������յ�
	int rowStart,rankStart,rowEnd,rankEnd;
	
	//�����ĺ��յ���к���
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
		//��ԭ���ĵ�·��Ҫɾ��
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
		ClearList(listStart);
		ClearList(listEnd);
		delete nodeEnd;
		return ;
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
	//����һ����ֱ�ӱ��Ŀ���
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
		//����ǵڼ��еڼ���
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

	//����������ӵ���������
	return (int)sqrt((float)((x1-x2)*(x1-x2)+(y1-y2)*(y1-y2)));
}

//�������Ƿ��Ѿ����б���
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
		//�Ƚ�fֵ�������ǰ�����С��tempF
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
			if (rowTemp < 0 || rankTemp < 0 || 
				rowTemp >= m_mapEdit.m_mapRow || rankTemp>=m_mapEdit.m_mapRank)
			{
				continue;
			}
			//�赲��
			if (m_PathLattice[rowTemp][rankTemp] == 1)
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