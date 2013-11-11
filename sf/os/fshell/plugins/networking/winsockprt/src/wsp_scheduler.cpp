// wsp_scheduler.cpp
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


#include "wsp_scheduler.h"
#include "wsp_panic.h"
#include <windows.h>
#include <emulator.h>

CWin32Scheduler* CWin32Scheduler::NewL()
	{
	return new(ELeave) CWin32Scheduler();
	}

CWin32Scheduler::~CWin32Scheduler()
	{
	__ASSERT_DEBUG(iActiveQue.IsEmpty(), Panic(EWinSockPrtActiveQueueNotEmpty));
	}

void CWin32Scheduler::Add(CWin32ActiveObject& aActiveObject)
	{
	iActiveQue.AddLast(aActiveObject);
	RebuildEventList();
	}

void CWin32Scheduler::Remove(CWin32ActiveObject& aActiveObject)
	{
	iActiveQue.Remove(aActiveObject);
	RebuildEventList();
	}

TBool CWin32Scheduler::IsPresent(CWin32ActiveObject& aActiveObject) const
	{
	TSglQueIter<CWin32ActiveObject> iterator(const_cast<CWin32Scheduler*>(this)->iActiveQue);
	iterator.SetToFirst();
	CWin32ActiveObject* thisActiveObject = NULL;
	while ((thisActiveObject = iterator++) != NULL)	
		{
		if (thisActiveObject == &aActiveObject)
			{
			return ETrue;
			}
		}
	return EFalse;
	}

void CWin32Scheduler::Start()
	{
	while (!iStopping)
		{
		Emulator::Escape();
		DWORD ret = WaitForMultipleObjectsEx(iNumEvents,  iEventList, FALSE, INFINITE, TRUE);
		Emulator::Reenter();
		if ((ret >= WAIT_OBJECT_0) && (ret < (WAIT_OBJECT_0 + iNumEvents)))
			{
			iActiveObjectList[ret - WAIT_OBJECT_0]->Run();
			}
		else if (ret == WAIT_IO_COMPLETION)
			{
			// TODO: Safe to ignore?
			}
		else
			{
			__ASSERT_DEBUG(EFalse, Panic(EWinSockPrtUnexpectedWaitError));
			}
		}
	}

void CWin32Scheduler::Stop()
	{
	iStopping = ETrue;
	}

CWin32Scheduler::CWin32Scheduler()
	: iActiveQue(_FOFF(CWin32ActiveObject, iLink))
	{
	}

void CWin32Scheduler::RebuildEventList()
	{
	iNumEvents = 0;
	TSglQueIter<CWin32ActiveObject> iterator(iActiveQue);
	iterator.SetToFirst();
	CWin32ActiveObject* thisActiveObject = NULL;
	while ((thisActiveObject = iterator++) != NULL)	
		{
		iActiveObjectList[iNumEvents] = thisActiveObject;
		iEventList[iNumEvents++] = thisActiveObject->iEvent;
		__ASSERT_DEBUG(iNumEvents <= MAXIMUM_WAIT_OBJECTS, Panic(EWinSockPrtTooManyWaitObjects));
		}
	}