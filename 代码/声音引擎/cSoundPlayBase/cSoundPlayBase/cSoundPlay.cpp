#include "StdAfx.h"
#include "cSoundPlay.h"
#include "mmsystem.h"
#include "digitalv.h"
#pragma comment(lib,"winmm.lib")


cSoundPlay::cSoundPlay(void)
{
	m_bgSoundID= 0;
	AddTimer(Timer_OpenCheck,50);
	AddTimer(Timer_CloseCheck,50);
	m_synSoundVolume = 1000;
	m_bSetVolume = false;
}


cSoundPlay::~cSoundPlay(void)
{
}

DWORD cSoundPlay::PlaySoundBG(wchar_t* pPath)
{
	if (pPath == NULL)
	{
		return 0;
	}
	if (m_bgSoundID != 0)
	{
		//如果当前有声音在播放，关闭当前的声音
		StopSoundBG();
	}
	MCI_OPEN_PARMS mciOpen;
	mciOpen.lpstrDeviceType = L"mpegvideo";
	mciOpen.lpstrElementName = pPath;
	MCIERROR mciErrro;
	mciErrro = mciSendCommand(0,MCI_OPEN,MCI_OPEN_TYPE | MCI_OPEN_ELEMENT,
		(DWORD)&mciOpen);
	if (mciErrro)
	{
		MessageBox(NULL,L"MCI_OPEN failed",L"error",MB_OK);
		return 0;
	}
	m_bgSoundID = mciOpen.wDeviceID;
	MCI_PLAY_PARMS mciPlay;
	mciErrro = mciSendCommand(m_bgSoundID,MCI_PLAY,MCI_DGV_PLAY_REPEAT,
		(DWORD)&mciPlay);
	if (mciErrro)
	{
		MessageBox(NULL,L"MCI_Play failed",L"error",MB_OK);
		return 0;
	}
	return m_bgSoundID;
}

void cSoundPlay::StopSoundBG()			//停止播放背景音乐
{
	if (m_bgSoundID != 0)
	{
		//如果有背景音乐在播放
		MCI_GENERIC_PARMS gp;
		gp.dwCallback = NULL;
		mciSendCommand(m_bgSoundID,MCI_CLOSE,MCI_WAIT,(DWORD)&gp);
		m_bgSoundID = 0;
	}
}

void cSoundPlay::PauseSoundBG()		//暂停背景音乐
{
	if (m_bgSoundID == 0)
	{
		return;
	}
	MCI_PLAY_PARMS playParams;
	mciSendCommand(m_bgSoundID,MCI_PAUSE,MCI_NOTIFY,(DWORD)&playParams);
}

void cSoundPlay::ReplaySoundBG()		//恢复播放音乐
{
	if (m_bgSoundID == 0)
	{
		return;
	}
	MCI_PLAY_PARMS playParams;
	mciSendCommand(m_bgSoundID,MCI_PLAY,MCI_DGV_PLAY_REPEAT,(DWORD)&playParams);
}

//特效声音只播放一次
//检测声音是否播放完，手动把设备停止（定时器里检测，
//所以播放声音也要在定时器的那个线程）
void cSoundPlay::PlaySynSound(wchar_t* pPath)
{
	if (pPath == NULL)
	{
		return;
	}
	m_willPlaySound.push_back(pPath);
}

void cSoundPlay::SetSoundVolumeBG(DWORD value)
{
	if (value > 1000)
	{
		value = 1000;
	}
	if (m_bgSoundID == 0)
	{
		return;
	}
	MCI_DGV_SETAUDIO_PARMS mciSetVolume;
	mciSetVolume.dwItem = MCI_DGV_SETAUDIO_VOLUME;
	mciSetVolume.dwValue =value;	//音量值 0 -1000
	MCIERROR err = mciSendCommand(m_bgSoundID,MCI_SETAUDIO,MCI_DGV_SETAUDIO_VALUE |
		MCI_DGV_SETAUDIO_ITEM,(DWORD)(LPVOID)&mciSetVolume);
	err = GetLastError();
	int a = 0;
}

void cSoundPlay::StopSynSound()
{

}

void cSoundPlay::PauseSynSound()
{

}

void cSoundPlay::RepaySynSound()
{

}

void cSoundPlay::SetSynSoundVolume(DWORD value)
{
	if (m_synSoundVolume == value)
	{
		return;
	//	m_synSoundVolume = value;
	}
	m_bSetVolume = true;
	m_synSoundVolume = value;
}



int cSoundPlay::OnTimer(int id,int iParam,string str)
{
	switch(id)
	{
	case Timer_OpenCheck:
		for (list<wstring>::iterator it = m_willPlaySound.begin();
			it != m_willPlaySound.end();it++)
		{
			DWORD id = 0;
			MCI_OPEN_PARMS mciOpen;
			mciOpen.lpstrDeviceType = L"mpegvideo";
			mciOpen.lpstrElementName = it->c_str();
			MCIERROR mciError;
			mciError = mciSendCommand(0,MCI_OPEN,MCI_OPEN_TYPE|
				MCI_OPEN_ELEMENT,(DWORD)&mciOpen);

			if (mciError)
			{
				MessageBox(NULL,L"MCI_Open syn failed",L"error",MB_OK);
				return 1;
			}
			id = mciOpen.wDeviceID;
			MCI_PLAY_PARMS mciPlay;
			mciError = mciSendCommand(id,MCI_PLAY,MCI_NOTIFY,(DWORD)&mciPlay);
			if (mciError)
			{
				MessageBox(NULL,L"MCI_Play syn failed",L"error",MB_OK);
				return 1;
			}
			m_synSoundID.push_back(id);
		}
		m_willPlaySound.clear();
		break;
	case Timer_CloseCheck:
		for (list<DWORD>::iterator it = m_synSoundID.begin();
			it != m_synSoundID.end();)
		{
			MCI_STATUS_PARMS mciStatusParams;
			mciStatusParams.dwItem = MCI_STATUS_LENGTH;
			mciStatusParams.dwReturn = 0;
			MCIERROR err = mciSendCommand(*it,MCI_STATUS,MCI_STATUS_ITEM,
				(DWORD)&mciStatusParams);
			if (err)
			{
				it++;
				continue;
			}
			DWORD allLen = mciStatusParams.dwReturn;//总时长
			mciStatusParams.dwItem = MCI_STATUS_POSITION;	

			err = mciSendCommand(*it,MCI_STATUS,MCI_STATUS_ITEM,
				(DWORD)&mciStatusParams);
			if (err)
			{
				it++;
				continue;
			}
			DWORD curLen = mciStatusParams.dwReturn;	//当前播放的时长 ms

			if (curLen == allLen)
			{
				//说明已经播放完成了、
				MCI_GENERIC_PARMS gp;
				gp.dwCallback = NULL;
				mciSendCommand(*it,MCI_CLOSE,MCI_WAIT,(DWORD)&gp);
				it = m_synSoundID.erase(it);
				continue;
			}

			if (m_bSetVolume)
			{
				MCI_DGV_SETAUDIO_PARMS mciSetVolume;
				mciSetVolume.dwItem = MCI_DGV_SETAUDIO_VOLUME;
				mciSetVolume.dwValue =m_synSoundVolume;	//音量值 0 -1000
				MCIERROR err = mciSendCommand(*it,MCI_SETAUDIO,MCI_DGV_SETAUDIO_VALUE |
					MCI_DGV_SETAUDIO_ITEM,(DWORD)(LPVOID)&mciSetVolume);
				m_bSetVolume = false;
			}

			it++;
		}
		break;
	}
	return 1;
}