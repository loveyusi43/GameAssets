#pragma once
#include "cMyTimer.h"

typedef void (*EventFunc)(void);

namespace myui
{
	//控件类型的枚举
	enum ControlType
	{
		type_picture,
		type_text,
		type_animation,
		type_button,
		type_num
	};

	//事件类型
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

		//设置控件位置
		void SetPos(tagPOINT pt);
		//获得控件位置
		tagPOINT GetPos();

		//获得控件类型
		ControlType GetControlType(){return m_type;}

		//获得控件矩形区域
		RECT GetRect();

		//获得宽度
		virtual unsigned GetWidth(){return 0;}
		//获得高度
		virtual unsigned GetHeight(){return 0;}

	protected:
		tagPOINT m_ptPos;	//控件的位置
		ControlType m_type;	//控件类型
	};

	class cPicture:public cBaseUI
	{
	public:
		cPicture();
		~cPicture();

		//创建图片
		bool Create(LPCSTR lpFilePath);

		//获得图片位图
		HBITMAP GetHBitmap();

		//获得位图宽度
		unsigned GetWidth();
		//获得位图的高度
		unsigned GetHeight();
	private:
		HBITMAP m_hBitmap;	//保存位图
	};

	//文字控件类型
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
		//文字属性：字体大小，字体颜色
		//创建文字
		bool Create(LPCSTR lpText,UINT testColor = RGB(0,0,0),
			TextType textType = text_normal);

		//获得文字类型
		TextType GetTextType();

		//获得文字颜色
		UINT GetTextColor();
		//设置文字颜色
		void SetTextColor(UINT rgbColor);

		//获得字体大小
		UINT GetTextSize();

		//设置字体大小
		void SetTextSize(UINT size);

		//获得文字
		LPCSTR GetText();

		//获得宽度
		unsigned GetWidth();
		//获得高度
		unsigned GetHeight();
		//设置宽度
		void SetWidth(UINT width);
		//设置高度
		void SetHeight(UINT height);

		tagPOINT m_pos;		//文字相对于控件的位置
	private:
		string m_text;	//文字内容
		UINT m_textColor;	//文字颜色
		TextType m_textType;	//文字类型
		UINT m_textSize;	//字体大小
		

		UINT m_width;	//宽度
		UINT m_height;	//高度
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

		//三个状态（正常状态，鼠标在按纽上，鼠标按下），事件响应

		//创建一个BUTTON
		bool Create(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,
			LPCSTR lpPushDownImagePath);

		//获得BUTTON状态
		ButtonStatus GetStatus();
		//设置状态
		void SetStatus(ButtonStatus status);

		//根据BUTTON状态获得要显示的状态图
		HBITMAP GetCurStatusHBitmap();

		//获得宽度
		unsigned GetWidth();
		//获得高度
		unsigned GetHeight();
	private:
		HBITMAP m_hBitmapNormal;
		HBITMAP m_hBitmapMouseOn;
		HBITMAP m_hBitmapPushDown;

		ButtonStatus m_status;

	};

	//事件信息结构体
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

		//定时器回调函数
		virtual int OnTimer(int id,int iParam,string str);

		//图片，文字，按纽，动画等控件
		//特效等属性
		
		
		//通过图片路径名创建图片
		cPicture* CreatePicture(LPCSTR lpFilePath);
		///创建文字
		cText* CreateText(LPCSTR lpText,UINT testColor=RGB(0,0,0),TextType textType=text_normal);
		//创建按纽
		cButton* CreateButton(LPCSTR lpNormalImagePath,LPCSTR lpMouseOnImagePath,
			LPCSTR lpPushDownImagePath);

		//控件绘制
		void Draw();
		//绘制文字
		void DrawText(cText* pText,HDC memDc);

		//获得绑定的窗口句柄
		HWND GetBindHwnd();

		//左键点击消息事件
		void OnLButtonDown();

		//左键弹起
		void OnLButtonUp();
		//鼠标移动
		void OnMouseMove();

		//检测是否从按纽上弹起
		void CheckButtonUp();

		//检测是否移动到按纽
		void CheckOnButton();

		//检测是否在按纽上按下
		void CheckClickButton();

		//为控件添加事件响应
		void AddEvent(cBaseUI* pUI,EventType eventType,EventFunc func);

	private:
		HWND m_hWnd;	//绑定的窗口
		list<cBaseUI*> m_uiList;	//保存所有控件
		list<stEvent> m_eventList;	//保存事件
	};
}

myui::cUIManager* CreateUI(HWND hWnd);