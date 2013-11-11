// wsp_scheduler.h
// 
// Copyright (c) 2002 - 2010 Accenture. All rights reserved.
// This component and the accompanying materials are made available
// under the terms of the "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
// 
// Initial Contributors:
// Accenture - Initial contribution
//

#ifndef __WSP_SCHEDULER_H__
#define __WSP_SCHEDULER_H__

#include "wsp_active.h"

const TInt KMaxWaitObjects = 64; // Should be MAXIMUM_WAIT_OBJECTS from windows.h, but this header doesn't sit happily with winsock2.h.


class CWin32Scheduler : public CBase
	{
public:
	static CWin32Scheduler* NewL();
	~CWin32Scheduler();
	void Add(CWin32ActiveObject& aActiveObject);
	void Remove(CWin32ActiveObject& aActiveObject);
	TBool IsPresent(CWin32ActiveObject& aActiveObject) const;
	void Start();
	void Stop();
private:
	CWin32Scheduler();
	void RebuildEventList();
private:
	TBool iStopping;
	TInt iNumEvents;
	WIN32_HANDLE iEventList[KMaxWaitObjects];
	CWin32ActiveObject* iActiveObjectList[KMaxWaitObjects];
	TSglQue<CWin32ActiveObject> iActiveQue;
	};


#endif // __WSP_SCHEDULER_H__
