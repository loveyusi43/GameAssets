// GameServer.cpp : �������̨Ӧ�ó������ڵ㡣
//

#include "stdafx.h"
#include "windows.h"
#include "process.h"
#include "iostream"
#include "list"
#include "string"
using namespace std;
#pragma comment(lib,"ws2_32.lib")

enum Message_Type
{
	Message_Min,
	Message_Login,
	Message_LoginOut,
	Message_Chat,
	Message_ChatToMe,
	Message_ChatToOther,
	Message_Max
};

struct stUserInfo
{
	SOCKET sock;
	wstring strName;
};

list<stUserInfo> g_listUser;	//�����������ߵ��û���Ϣ

bool IsExit(wstring name)
{
	for (list<stUserInfo>::iterator it = g_listUser.begin();
		it != g_listUser.end();it++)
	{
		if (it->strName == name)
		{
			return true;
		}
	}
	return false;
}

wstring GetNewName(stUserInfo userInfo)
{
	wchar_t newName[100];
	int index = 1;
	while(1)
	{
		wsprintf(newName,L"%s%d",userInfo.strName.c_str(),index);
		if (!IsExit(newName))
		{
			break;
		}
		index++;
	}
	wstring str = newName;
	return str;
}

void SendToClient(Message_Type msgType,SOCKET sock,wstring str)
{
	if (msgType <= Message_Min || msgType >= Message_Max)
	{
		return;
	}
	wchar_t sendBuf[1024];
	switch(msgType)
	{
	case Message_Login:
		{
			wsprintf(sendBuf,L"%d_%s",msgType,str.c_str());
			break;
		}
	case Message_LoginOut:
		{
			wsprintf(sendBuf,L"%d_%s",msgType,str.c_str());
			break;
		}
	case Message_ChatToMe:
	case Message_ChatToOther:
		{
			wsprintf(sendBuf,L"%d_%s",msgType,str.c_str());
			break;
		}
	}
	send(sock,(char*)sendBuf,lstrlen(sendBuf)*2,0);
}

void SyncUserInfo(stUserInfo userinfo)
{
	//1.���ҵ���Ϣͬ����������
	//2.����������Ϣͬ������
	for (list<stUserInfo>::iterator it = g_listUser.begin();
		it != g_listUser.end();it++)
	{
		if (userinfo.sock == it->sock)
		{
			continue;
		}
		SendToClient(Message_Login,userinfo.sock,it->strName);
		Sleep(10);
		SendToClient(Message_Login,it->sock,userinfo.strName);
	}
}

bool GetUserByName(wstring name,stUserInfo& userinfo)
{
	for (list<stUserInfo>::iterator it = g_listUser.begin();
		it != g_listUser.end();it++)
	{
		if (it->strName == name)
		{
			userinfo = *it;
			return true;
		}
	}
	return false;
}

bool GetUserBySocket(SOCKET sock,stUserInfo& userinfo)
{
	for (list<stUserInfo>::iterator it = g_listUser.begin();
		it != g_listUser.end();it++)
	{
		if (it->sock == sock)
		{
			userinfo = *it;
			return true;
		}
	}
	return false;
}

void Chat(SOCKET sockSend,wstring nameSendTo,wstring strText)
{
	//����Ϣ���͸�Ŀ����
	stUserInfo userSendTo;
	if(!GetUserByName(nameSendTo,userSendTo))
	{
		return;
	}
	stUserInfo userSend;
	if (!GetUserBySocket(sockSend,userSend))
	{
		return;
	}
	wstring strBuf;
	strBuf = userSend.strName + L"_" +userSendTo.strName + L"_"+strText;
	SendToClient(Message_ChatToMe,userSendTo.sock,strBuf);
	SendToClient(Message_ChatToOther,userSend.sock,strBuf);
}

void RecvMessage(SOCKET sock,wstring strBuf)
{
	//��ò�������
	int pos = strBuf.find('_');
	int msgType = Message_Min;
	if (pos != -1)
	{
		wstring bufTemp = strBuf.substr(0,pos);
		strBuf = strBuf.substr(pos+1);
		char* temp = (char*)bufTemp.c_str();
		msgType = atoi(temp);
	}
	else
	{
		char* temp = (char*)strBuf.c_str();
		msgType = atoi(temp);
	}
	if (msgType <= Message_Min || msgType >= Message_Max)
	{
		return;
	}
	switch(msgType)
	{
	case Message_Login:
		{
			//������Ϣ������SOCKET��Ϣ������
			stUserInfo userInfo;
			userInfo.sock = sock;
			userInfo.strName = strBuf;
			if (IsExit(userInfo.strName))
			{
				userInfo.strName = GetNewName(userInfo);
			}
			g_listUser.push_back(userInfo);
			//ͬ����Ϣ
			SyncUserInfo(userInfo);
		}
	case Message_Chat:
		{
			//��÷�����Ϣ�����˺�������Ϣ
			pos = strBuf.find('_');
			if (pos == -1)
			{
				return;
			}
			wstring strName = strBuf.substr(0,pos);
			wstring strText = strBuf.substr(pos+1);
			Chat(sock,strName,strText);
			break;
		}
	}
}

void LoginOut(SOCKET sock)
{
	for (list<stUserInfo>::iterator it = g_listUser.begin();
		it != g_listUser.end();it++)
	{
		if (it->sock == sock)
		{
			//����Ϣ���͸������ˣ�����������
			for (list<stUserInfo>::iterator it2 = g_listUser.begin();
				it2 != g_listUser.end();it2++)
			{
				if (it2->sock != it->sock)
				{
					SendToClient(Message_LoginOut,
						it2->sock,it->strName);
				}
			}
			g_listUser.erase(it);
			break;
		}
	}
}

//�������ݵ��߳�
void receive(PVOID param)
{
	SOCKET sock = *((SOCKET*)param);
	char buf[2048] = {0};
	int bytes;
	while(1)
	{
		//��������
		if ((bytes = recv(sock,buf,sizeof(buf),0))== SOCKET_ERROR)
		{
			LoginOut(sock);
			_endthread();
			return;
		}
		buf[bytes] = '\0';
		wchar_t bufTest[1024];
		memcpy(bufTest,buf,bytes);
		bufTest[bytes/2] = '\0';
		RecvMessage(sock,bufTest);
	}
}

void ReceiveConnectThread(void* param)
{
	SOCKET socketServer = *((SOCKET*)param);
	while(1)
	{
		SOCKET clientSocket;	//�������տͻ�������
		struct sockaddr_in clientAddress;	//�׽ӵĵ�ַ���˿�
		memset(&clientAddress,0,sizeof(clientAddress));
		int addrLen = sizeof(clientAddress);
		if (INVALID_SOCKET ==(clientSocket = accept(socketServer,(sockaddr*)&clientAddress,&addrLen)))
		{
			cout << "���ܿͻ�������ʧ��";
			return ;
		}
		

		_beginthread(receive,0,&clientSocket);


	}
}

SOCKET StartServer()
{
	SOCKET serverSocket;
	if ((serverSocket = socket(AF_INET,SOCK_STREAM,IPPROTO_TCP)) == INVALID_SOCKET)
	{
		cout << "�����׽���ʧ��";
		return -1;
	}

	struct sockaddr_in serverAddress;
	memset(&serverAddress,0,sizeof(sockaddr_in));
	serverAddress.sin_family = AF_INET;
	serverAddress.sin_addr.S_un.S_addr = htonl(INADDR_ANY);
	serverAddress.sin_port = htons(1986);

	//��
	if (bind(serverSocket,(sockaddr*)&serverAddress,sizeof(serverAddress))== SOCKET_ERROR)
	{
		cout << "�׽��ְ�ʧ��" << endl;
		return 0;
	}

	//����
	if (listen(serverSocket,SOMAXCONN) == SOCKET_ERROR)
	{
		cout << "����ʧ��";
		return 0;
	}

	return serverSocket;
}

int _tmain(int argc, _TCHAR* argv[])
{
	WSADATA wsa;
	if (WSAStartup(MAKEWORD(2,2),&wsa) != 0)
	{
		cout << "�׽��ֳ�ʼ��ʧ��";
		return -1;
	}

	SOCKET sock = StartServer();//����������

	_beginthread(ReceiveConnectThread,0,&sock);

	char buf[1024];
	while(1)
	{
		gets(buf);
	}
	return 0;
}

