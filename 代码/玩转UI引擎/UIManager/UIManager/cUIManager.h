#pragma once
#include "cMyTimer.h"

typedef void (*EventFunc)(void);

namespace myui
{
	//�ؼ����͵�ö��
	enum ControlType
	{
		type_picture,
		type_text,
		type_animation,
		type_button,
		type_num
	};

	//�¼�����
	enum EventType
	{
		event_lbuttondown,
		event_lbuttonup
	};

	class cBaseUI
	{
	public:
		cBaseUI();
		virtual ~cBaseUI();

		//���ÿؼ�λ��
		void SetPos(tagPOINT pt);
		//��ÿؼ�λ��
		tagPOINT GetPos();

		//��ÿؼ�����
		ControlType GetControlType(){return m_type;}

		//��ÿؼ���������
		RECT GetRect();

		//��ÿ��
		virtual unsigned GetWidth(){return 0;}
		//��ø߶�
		virtual unsigned GetHeight(){return 0;}

	protected:
		tagPOINT m_ptPos;	//�ؼ���λ��
		ControlType m_type;	//�ؼ�����
	};

	class cPicture:public cBaseUI
	{
	public:
		cPicture();
		~cPicture();

		//����ͼƬ
		bool Create(LPCSTR lpFilePath);

		//���ͼƬλͼ
		HBITMAP GetHBitmap();

		//���λͼ���
		unsigned GetWidth();
		//���λͼ�ĸ߶�
		unsigned GetHeight();
	private:
		HBITMAP m_hBitmap;	//����λͼ
	};

	//�¼���Ϣ�ṹ��
	struct stEvent
	{
		EventType eventType;
		cBaseUI* pUI;
		EventFunc func;
	};

	class cUIManager:public cMyTimer
	{
	public:
		cUIManager(HWND hWnd);
		virtual ~cUIManager();

		//��ʱ���ص�����
		virtual int OnTimer(int id,int iParam,string str);

		//ͼƬ�����֣���Ŧ�������ȿؼ�
		//��Ч������
		
		
		//ͨ��ͼƬ·��������ͼƬ
		cPicture* CreatePicture(LPCSTR lpFilePath);

		//�ؼ�����
		void Draw();

		//��ð󶨵Ĵ��ھ��
		HWND GetBindHwnd();

		//��������Ϣ�¼�
		void OnLButtonDown();

		//Ϊ�ؼ�����¼���Ӧ
		void AddEvent(cBaseUI* pUI,EventType eventType,EventFunc func);

	private:
		HWND m_hWnd;	//�󶨵Ĵ���
		list<cBaseUI*> m_uiList;	//�������пؼ�
		list<stEvent> m_eventList;	//�����¼�
	};
}

myui::cUIManager* CreateUI(HWND hWnd);