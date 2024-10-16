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

		//控件绘制
		void Draw();

		//获得绑定的窗口句柄
		HWND GetBindHwnd();

		//左键点击消息事件
		void OnLButtonDown();

		//为控件添加事件响应
		void AddEvent(cBaseUI* pUI,EventType eventType,EventFunc func);

	private:
		HWND m_hWnd;	//绑定的窗口
		list<cBaseUI*> m_uiList;	//保存所有控件
		list<stEvent> m_eventList;	//保存事件
	};
}

myui::cUIManager* CreateUI(HWND hWnd);