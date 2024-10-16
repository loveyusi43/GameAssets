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

	DWORD PlaySoundBG(wchar_t* pPath);	//���ű�������
	void StopSoundBG();			//ֹͣ���ű�������
	void PauseSoundBG();		//��ͣ��������
	void ReplaySoundBG();		//�ָ���������
	void SetSoundVolumeBG(DWORD value);			//���ñ�����������

	void PlaySynSound(wchar_t* pPath);	//������Ч
	void SetSynSoundVolume(DWORD value);	//������Ч��������

	void StopSynSound();
	void PauseSynSound();
	void RepaySynSound();
public:
	DWORD m_bgSoundID;	//��ǰ���ŵı�������ID

	list<wstring> m_willPlaySound;	//��Ҫ���ŵ�����
	list<DWORD> m_synSoundID;		//���ڲ��ŵ���Ч�����б�

	DWORD m_synSoundVolume;		//��Ч������С
	bool m_bSetVolume;			//�Ƿ���Ҫ������������
};

