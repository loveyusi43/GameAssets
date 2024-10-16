#pragma once
#include "cGameSenceBase.h"

enum Role_Status
{
	Status_Move,
	Status_Stop
};

class cRoleObject
{
public:
	cRoleObject(void);
	~cRoleObject(void);

	Role_Status m_roleStatus;
	ST_ANIPICTURE* m_pPictureInfo;
};

