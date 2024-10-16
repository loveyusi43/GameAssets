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

	//���ֿؼ�����
	enum TextType
	{
		text_normal,
		text_scoll,
		text_twinkle,
		textTypeNum
	};

	class cText:public cBaseUI
	{
	public:
		cText();
		~cText();
		//�������ԣ������С��������ɫ
		//��������
		bool Create(LPCSTR lpText,UINT testColor = RGB(0,0,0),
			TextType textType = text_normal);

		//�����������
		TextType GetTextType();

		//���������ɫ
		UINT GetTextColor();
		//����������ɫ
		void SetTextColor(UINT rgbColor);

		//��������С
		UINT GetTextSize();

		//���������С
		void SetTextSize(UINT size);

		//�������
		LPCSTR GetText();

		//��ÿ��
		unsigned GetWidth();
		//��ø߶�
		unsigned GetHeight();
		//���ÿ��
		void SetWidth(UINT width);
		//���ø߶�
		void SetHeight(UINT height);

		tagPOINT m_pos;		//��������ڿؼ���λ��
	private:
		string m_text;	//��������
		UINT m_textColor;	//������ɫ
		TextType m_textType;	//��������
		UINT m_textSize;	//�����С
		

		UINT m_width;	//���
		UINT m_height;	//�߶�
	};

	enum ButtonStatus
	{
		status_normal,
		status_mouseon,
		status_pushdown
	};

	class cButton:public cBaseUI
	{
	public:
		cButton();
		~cButton();

		//����״̬������״̬������ڰ�Ŧ�ϣ���갴�£����¼���Ӧ

		//����һ��BUTTON
		bool Create(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,
			LPCSTR lpPushDownImagePath);

		//���BUTTON״̬
		ButtonStatus GetStatus();
		//����״̬
		void SetStatus(ButtonStatus status);

		//����BUTTON״̬���Ҫ��ʾ��״̬ͼ
		HBITMAP GetCurStatusHBitmap();

		//��ÿ��
		unsigned GetWidth();
		//��ø߶�
		unsigned GetHeight();
	private:
		HBITMAP m_hBitmapNormal;
		HBITMAP m_hBitmapMouseOn;
		HBITMAP m_hBitmapPushDown;

		ButtonStatus m_status;

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
		///��������
		cText* CreateText(LPCSTR lpText,UINT testColor=RGB(0,0,0),TextType textType=text_normal);
		//������Ŧ
		cButton* CreateButton(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,
			LPCSTR lpPushDownImagePath);

		//�ؼ�����
		void Draw();
		//��������
		void DrawText(cText* pText,HDC memDc);

		//��ð󶨵Ĵ��ھ��
		HWND GetBindHwnd();

		//��������Ϣ�¼�
		void OnLButtonDown();

		//�������
		void OnLButtonUp();
		//����ƶ�
		void OnMouseMove();

		//����Ƿ�Ӱ�Ŧ�ϵ���
		void CheckButtonUp();

		//����Ƿ��ƶ�����Ŧ
		void CheckOnButton();

		//����Ƿ��ڰ�Ŧ�ϰ���
		void CheckClickButton();

		//Ϊ�ؼ�����¼���Ӧ
		void AddEvent(cBaseUI* pUI,EventType eventType,EventFunc func);

	private:
		HWND m_hWnd;	//�󶨵Ĵ���
		list<cBaseUI*> m_uiList;	//�������пؼ�
		list<stEvent> m_eventList;	//�����¼�
	};
}

myui::cUIManager* CreateUI(HWND hWnd);