#pragma once
#include "cMyTimer.h"

enum Timer_Type
{
	Timer_OpenCheck,
	Timer_CloseCheck
};
class cSoundPlay:public cMyTimer
{
public:
	cSoundPlay(void);
	~cSoundPlay(void);
	virtual int OnTimer(int id,int iParam,string str);

	DWORD PlaySoundBG(wchar_t* pPath);	//播放背景音乐
	void StopSoundBG();			//停止播放背景音乐
	void PauseSoundBG();		//暂停背景音乐
	void ReplaySoundBG();		//恢复播放音乐
	void SetSoundVolumeBG(DWORD value);			//设置背景音乐音量

	void PlaySynSound(wchar_t* pPath);	//播放音效
	void SetSynSoundVolume(DWORD value);	//设置音效声音音量

	void StopSynSound();
	void PauseSynSound();
	void RepaySynSound();
public:
	DWORD m_bgSoundID;	//当前播放的背景音乐ID

	list<wstring> m_willPlaySound;	//将要播放的声音
	list<DWORD> m_synSoundID;		//正在播放的特效声音列表

	DWORD m_synSoundVolume;		//音效声音大小
	bool m_bSetVolume;			//是否需要重新设置音量
};

